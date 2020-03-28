#ifndef ZW_DARK
#define ZW_DARK

#include <opencv2/opencv.hpp>

#include <cuda_runtime.h>
#include <curand.h>
#include <cublas_v2.h>
#include <cudnn.h>

#define GPU
#define CUDNN

extern "C"
{
#include <darknet.h>
}

namespace zw
{
/** @brief a base wrapper of darknet network
*/
class DarkModel
{
private:
    char *m_cfg;
    char *m_weight;

public:
    network net;
    int netw, neth;

    DarkModel(char *cfg, char *weight, int gpu_id = 0);
    ~DarkModel();

    void init();
    void destroy();
};

DarkModel::DarkModel(char *cfg, char *weight, int gpu_id)
{
    m_cfg = cfg;
    m_weight = weight;
    cuda_set_device(gpu_id);
    srand(2222222);
}

DarkModel::~DarkModel()
{
}

void DarkModel::init()
{
    net = *load_network_custom(m_cfg, m_weight, 0, 1);
    fuse_conv_batchnorm(net);
    calculate_binary_weights(net);
    netw = network_width(&net);
    neth = network_height(&net);
}

void DarkModel::destroy()
{
    free_network(net);
}
} // namespace zw
#endif // ZW_DARK