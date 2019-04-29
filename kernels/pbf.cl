#define PI 3.14159265358979323846264338327950288f
#define EPS_F 1e-22f

typedef struct fluid_params {
    uchar solver_iterations;
    float dt;
    float gravity;
    float kernel_h;
    float rest_density;
    float desity_eps;
    float s_corr_k;
    float s_corr_dq_multiplier;
    float s_corr_n;
    float vort_eps;
    float visc_c;

    float grid_res;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;

    unsigned dim_x;
    unsigned dim_y;
    unsigned dim_z;
} fluid_params;

unsigned bin(unsigned dim_x, unsigned dim_y, uint3 b);
uint3 bin_index(unsigned dim_x, unsigned dim_y, unsigned dim_z, float bin_res, float3 p);

float poly6(float3 ri, float h);
float3 grad_spiky(float3 ri, float h);

__kernel
void predict_position(const fluid_params params,
                __global const float *pos,
                __global const float *vel,
                __global float *pred_pos) {

    size_t id = get_global_id(0);
    float3 pos_i = vload3(id, pos);
    float3 vel_i = vload3(id, vel);
    vel_i.y += params.gravity * params.dt;

    float3 pred = pos_i + vel_i * params.dt;

    if (pred.y < EPS_F) {
        pred.y = EPS_F;
    }

    vstore3(pred, id, pred_pos);
}

__kernel
void count_bins(const fluid_params params,
                __global const float *pred_pos,
                __global unsigned *bin_of,
                __global unsigned *bin_offset,
                __global unsigned *bin_counts) {

    size_t id = get_global_id(0);
    float3 pred_pos_i = vload3(id, pred_pos);

    uint3 b_index = bin_index(params.dim_x, params.dim_y, params.dim_z, params.grid_res, pred_pos_i);
    unsigned b = bin(params.dim_x, params.dim_y, b_index);

    bin_of[id] = b;
    bin_offset[id] = atomic_inc(&bin_counts[b]);
}

__kernel
void prefix_sum(__global const unsigned *bin_counts,
                __global unsigned *bin_starts) {

    size_t id = get_global_id(0);
    unsigned acc = 0;

    for (unsigned i = 0; i < id; ++i)
        acc += bin_counts[i];

    bin_starts[id] = acc;
}

__kernel
void reindex_particles(__global const unsigned *bin_starts,
                __global const unsigned *bin_of,
                __global const unsigned *bin_offset,
                __global const float *pos,
                __global const float *vel,
                __global const float *pred_pos,
                __global unsigned *ind_bin_of,
                __global float *ind_pos,
                __global float *ind_vel,
                __global float *ind_pred_pos) {

    unsigned id_old = get_global_id(0);
    unsigned b = bin_of[id_old];

    unsigned id_new = bin_starts[b] + bin_offset[id_old];
    ind_bin_of[id_new] = b;
    vstore3(vload3(id_old, pos), id_new, ind_pos);
    vstore3(vload3(id_old, vel), id_new, ind_vel);
    vstore3(vload3(id_old, pred_pos), id_new, ind_pred_pos);

}

__kernel
void calculate_lambda(const fluid_params params,
                __global const unsigned *bin_starts,
                __global const unsigned *bin_counts,
                __global const float *pred,
                __global float *lambda) {

    unsigned id_i = get_global_id(0);
    float3 pos_i = vload3(id_i, pred);

    unsigned num_bins = params.dim_x * params.dim_y * params.dim_z;
    uint3 b_index = bin_index(params.dim_x, params.dim_y, params.dim_z, params.grid_res, pos_i);

    uint3 n_bin_index;
    unsigned x, y, z, n_bin, n_bin_start, n_bin_end, id_j;
    float3 pos_j, grad_ci_pk;

    float3 grad_ci_pi = float3(0.f, 0.f, 0.f);
    float density = 0.f;
    float denom = 0.f;

    for (x = -1; x <= 1; ++x) {
        for (y = -1; y <= 1; ++y) {
            for (z = -1; z <= 1; ++z) {
                n_bin_index = b_index + int3(x, y, z);
                n_bin = bin(params.dim_x, params.dim_y, n_bin_index);

                if (n_bin >= num_bins)
                    continue;

                n_bin_start = bin_starts[n_bin];
                n_bin_end = n_bin_start + bin_counts[n_bin];

                for (id_j = n_bin_start; id_j < n_bin_end; ++id_j) {
                    pos_j = vload3(id_j, pred);
                    density += poly6(pos_i - pos_j, params.kernel_h);

                    grad_ci_pk = grad_spiky(pos_i - pos_j, params.kernel_h);
                    if (id_i != id_j)
                        denom += dot(grad_ci_pk, grad_ci_pk);
                    grad_ci_pi += grad_ci_pk;
                }
            }
        }
    }

    float num = density / params.rest_density - 1.f;
    denom += dot(grad_ci_pi, grad_ci_pi);
    lambda[id_i] = -num / (denom / pow(params.rest_density, 2.f) + params.desity_eps);
}

__kernel
void calculate_dp(const fluid_params params,
                __global const float *bin_starts,
                __global const float *bin_counts,
                __global const float *pred,
                __global const float *lambda,
                __global float *d_pos) {

    unsigned id_i = get_global_id(0);
    float3 pos_i = vload3(id_i, pred);
    float lambda_i = lambda[id_i];

    unsigned num_bins = params.dim_x * params.dim_y * params.dim_z;
    uint3 b_index = bin_index(params.dim_x, params.dim_y, params.dim_z, params.grid_res, pos_i);

    uint3 n_bin_index;
    unsigned x, y, z, n_bin, n_bin_start, n_bin_end, id_j;
    float lambda_j, s_corr;
    float3 pos_j, r;

    float3 acc = float3(0.f, 0.f, 0.f);
    float s_corr_mult = -params.s_corr_k / pow(
            poly6(float3(params.s_corr_dq_multiplier, 0.f, 0.f), params.kernel_h),
            params.s_corr_n);

    for (x = -1; x <= 1; ++x) {
        for (y = -1; y <= 1; ++y) {
            for (z = -1; z <= 1; ++z) {
                n_bin_index = b_index + int3(x, y, z);
                n_bin = bin(params.dim_x, params.dim_y, n_bin_index);

                if (n_bin >= num_bins)
                    continue;

                n_bin_start = bin_starts[n_bin];
                n_bin_end = n_bin_start + bin_counts[n_bin];

                for (id_j = n_bin_start; id_j < n_bin_end; ++id_j) {
                    pos_j = vload3(id_j, pred);
                    lambda_j = lambda[id_j];
                    r = pos_i - pos_j;
                    s_corr = s_corr_mult * pow(poly6(r, params.kernel_h), params.s_corr_n);
                    acc += (lambda_i + lambda_j + s_corr) * grad_spiky(r, params.kernel_h);
                }
            }
        }
    }

    vstore3(acc / params.rest_density, id_i, d_pos);
}

__kernel
void update_pred_position(const fluid_params params,
                __global const float *pred,
                __global const float *d_pos,
                __global float *new_pred) {
    size_t id = get_global_id(0);
    float3 pos_i = vload3(id, pred);
    float3 dp_i = vload3(id, d_pos);

    float3 pred_pos = clamp(
            pos_i + dp_i,
            float3(params.x_min, params.y_min, params.y_max),
            float3(params.x_max, params.y_max, params.z_max));

    vstore3(pred_pos, id, new_pred);
}

__kernel
void update_velocity(const fluid_params params,
                __global const float *pos,
                __global const float *pred_pos,
                __global float *vel) {

    size_t id = get_global_id(0);
    float3 pos_i = vload3(id, pos);
    float3 pred_pos_i = vload3(id, pred_pos);
    float3 v = (1.f / params.dt) * (pred_pos_i - pos_i);
    vstore3(v, id, vel);
}

unsigned bin(unsigned dim_x, unsigned dim_y, uint3 b) {
    return b.z * dim_x * dim_y + b.y * dim_x + b.x;
}

uint3 bin_index(unsigned dim_x, unsigned dim_y, unsigned dim_z, float bin_res, float3 p) {
    uint3 b;
    b.x = p.x / bin_res;
    b.y = p.y / bin_res;
    b.z = p.z / bin_res;
    return b;
}

float poly6(float3 ri, float h) {
    float r = length(ri);
    if (r >= h || r < EPS_F)
        return 0.f;
    return (315.f / (64.f * PI * pow(h, 9.f))) * pow(h * h - r * r, 3.f);
}

float3 grad_spiky(float3 ri, float h) {
    float r = length(ri);
    if (r >= h || r < EPS_F)
        return float3(0, 0, 0);
    return (-45.f / (PI * pow(h, 6.f))) * pow(h - r, 2.f) * normalize(ri);
}
