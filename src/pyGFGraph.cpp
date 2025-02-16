// #include <pybind11/pybind11.h>
// #include <pybind11/stl.h>
// #include "../include/interact_graph.hpp"
// #include "../include/louvain.hpp"

// namespace py = pybind11;
// using namespace gf_graph;

// PYBIND11_MODULE(pyGFGraph, m) {
//     m.doc() = "Python bindings for GFGraph using pybind11";

//     // 绑定 InteractGraph 类
//     py::class_<InteractGraph>(m, "InteractGraph")
//         .def(py::init<>())  // 默认构造函数
//         .def(py::init<const std::string&, const std::string&, const std::string&>())  // 参数化构造函数
//         .def("AddAccounts", &InteractGraph::AddAccounts)
//         .def("AddPosts", &InteractGraph::AddPosts)
//         .def("AddInteractions", &InteractGraph::AddInteractions)
//         .def("GetTopkAccounts", &InteractGraph::GetTopkAccounts)
//         .def("GetCommonInteractingAccounts", &InteractGraph::GetCommonInteractingAccounts)
//         .def("GetOldestPost",
//             py::overload_cast<const DateTime&, const DateTime&>(&gf_graph::InteractGraph::GetOldestPost, py::const_))
//         .def("GetOldestPost",
//             py::overload_cast<const std::string&, const std::string&>(&gf_graph::InteractGraph::GetOldestPost, py::const_))
//         .def("PostsByFreq", py::overload_cast<const TimeRange&, const DateTime&, const DateTime&, bool>(&InteractGraph::PostsByFreq, py::const_))
//         .def("PostsByFreq", py::overload_cast<const TimeRange&, const std::string&, const std::string&, bool>(&InteractGraph::PostsByFreq, py::const_));

//     // 绑定 LouvainCommunity 函数
//     m.def("LouvainCommunity", &LouvainCommunity, py::arg("graph"), py::arg("sorted") = true);
// }

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
//#include "../include/interact_graph.hpp"
//#include "../include/louvain.hpp"
#include "gf_graph.hpp" 

namespace py = pybind11;
using namespace gf_graph;

PYBIND11_MODULE(pyGFGraph, m) {
    m.doc() = "Python bindings for GFGraph using pybind11";

    // 绑定 InteractGraph 类
    py::class_<InteractGraph>(m, "InteractGraph")
        .def(py::init<>())  // 默认构造函数
        .def(py::init<const std::string&, const std::string&, const std::string&>())  // 参数化构造函数

        // 绑定 AddAccounts，支持第二个参数 replace，默认为 true
        .def("AddAccounts", &InteractGraph::AddAccounts, py::arg("account_path"), py::arg("replace") = true)

        // 绑定 AddPosts，支持第二个参数 replace，默认为 true
        .def("AddPosts", &InteractGraph::AddPosts, py::arg("post_path"), py::arg("replace") = true)

        // 绑定 AddInteractions，支持第二个参数 replace，默认为 true
        .def("AddInteractions", &InteractGraph::AddInteractions, py::arg("interact_path"), py::arg("replace") = true)

        // 绑定 GetTopkAccounts
        // .def("GetTopkAccounts", &InteractGraph::GetTopkAccounts, py::arg("topk"), py::arg("label"))
        .def("GetTopkAccounts",
            py::overload_cast<vid_t, topk_label_t>(&InteractGraph::GetTopkAccounts, py::const_),
            py::arg("topk"), py::arg("label"))
        // 增加int类型重载
        .def("GetTopkAccounts",
            py::overload_cast<vid_t, int>(&InteractGraph::GetTopkAccounts, py::const_),
            py::arg("topk"), py::arg("label"))

        // 绑定 GetCommonInteractingAccounts
        .def("GetCommonInteractingAccounts", &InteractGraph::GetCommonInteractingAccounts, py::arg("userIds"))

        // 绑定 GetOldestPost，处理函数重载
        .def("GetOldestPost",
            py::overload_cast<const DateTime&, const DateTime&>(&InteractGraph::GetOldestPost, py::const_),
            py::arg("start_time"), py::arg("end_time"))
        .def("GetOldestPost",
            py::overload_cast<const std::string&, const std::string&>(&InteractGraph::GetOldestPost, py::const_),
            py::arg("start_time"), py::arg("end_time"))

        // 绑定 PostsByFreq，处理函数重载
        .def("PostsByFreq",
            py::overload_cast<const TimeRange&, const DateTime&, const DateTime&, bool>(&InteractGraph::PostsByFreq, py::const_),
            py::arg("interval"), py::arg("start_time"), py::arg("end_time"), py::arg("sorted") = true)
        .def("PostsByFreq",
            py::overload_cast<const TimeRange&, const std::string&, const std::string&, bool>(&InteractGraph::PostsByFreq, py::const_),
            py::arg("interval"), py::arg("start_time"), py::arg("end_time"), py::arg("sorted") = true)
        // 增加第一个参数int类型重载
        .def("PostsByFreq",
            py::overload_cast<const uint64_t&, const std::string&, const std::string&, bool>(&InteractGraph::PostsByFreq, py::const_),
            py::arg("interval"), py::arg("start_time"), py::arg("end_time"), py::arg("sorted") = true)
        .def("PostsByFreq",
            py::overload_cast<const uint64_t&, const DateTime&, const DateTime&, bool>(&InteractGraph::PostsByFreq, py::const_),
            py::arg("interval"), py::arg("start_time"), py::arg("end_time"), py::arg("sorted") = true);

    // 绑定 LouvainCommunity 函数
    m.def("LouvainCommunity", &LouvainCommunity, py::arg("graph"), py::arg("sorted") = true);
}
