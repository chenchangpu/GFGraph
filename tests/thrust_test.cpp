#include <vector>

#include <thrust/system/omp/execution_policy.h>
#include <thrust/sequence.h>
#include <thrust/for_each.h>

#include "str.hpp"
#include "log.hpp"

using namespace std;

int main() {
    int len = 200;
    vector<int> indices(len);
    vector<int> vec(len, 0);

    thrust::sequence(thrust::omp::par, indices.begin(), indices.end());
    thrust::for_each(thrust::omp::par, indices.begin(), indices.end(), [&vec](const auto& x) {
        vec[x] = x * 2;
    });

    thrust::sort_by_key(
        thrust::omp::par,
        vec.begin(),
        vec.end(),
        indices.begin(),
        thrust::greater<int>()
    );

    printx("indices:", VecToString(indices));

    return 0;
}