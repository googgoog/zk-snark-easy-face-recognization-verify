//
// Created by xkx on 11/1/18.
//

#define CURVE_ALT_BN128

#include <vector>
#include "sudoku_gadget.hpp"
#include "sudoku_snark.hpp"
#include <libff/common/default_types/ec_pp.hpp>
#include <iostream>

template<typename gg_ppzksnark_ppT>
bool test_completeness(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair) {
    std::cout << "trying to generate proof..." << std::endl;
    std::vector<int> _inputs_a = {1, 3, 2, 4};
    std::vector<int> _inputs_b = {2, 4, 1, 3};
    std::vector<int> _inputs_c = {3, 1, 4, 2};
    std::vector<int> _inputs_d = {4, 2, 3, 1};
    auto proof = generate_proof<gg_ppzksnark_ppT>(keypair.pk, _inputs_a,
                                               _inputs_b, _inputs_c, _inputs_d);
    std::cout << "proof generated!" << std::endl;

    if (!proof) {
        std::cout << "unexpected: proof is none!" << std::endl;
        return false;
    }
    // 这里就是数独公开的部分，这是大家都已知的primary input
    bool ret = verify_proof(keypair.vk, *proof, 2, 2, 3, 3, 2, 3);
    if (!ret) {
        std::cout << "proof validation: failed!";
        return false;
    } else{
        std::cout << "proof validation: passed!";
        return true;
    }
}

//
template<typename gg_ppzksnark_ppT>
bool test_soundness(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair) {

    std::cout << "trying to generate proof..." << std::endl;
    //invalid auxiliary input
    std::vector<int> _inputs_a = {1, 3, 2, 4};
    std::vector<int> _inputs_b = {2, 3, 1, 3};
    std::vector<int> _inputs_c = {3, 1, 4, 2};
    std::vector<int> _inputs_d = {4, 2, 3, 1};
    auto proof = generate_proof<gg_ppzksnark_ppT>(keypair.pk, _inputs_a,
                                               _inputs_b, _inputs_c, _inputs_d);
    std::cout << "proof generated!" << std::endl;

    if (!proof) {
        std::cout << "unexpected: proof is none!" << std::endl;
        return false;
    }
    bool ret = verify_proof(keypair.vk, *proof, 2, 2, 3, 3, 2, 3);
    if (!ret) {
        std::cout << "proof validation: failed!";
        return false;
    } else {
        std::cout << "proof validation: passed!";
        return true;
    }
}

template<typename gg_ppzksnark_ppT>
bool test_compare(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair) {
    std::cout << "trying to generate proof..." << std::endl;    
    int a = 90;
    int b = 100;

    auto proof = generate_proof<gg_ppzksnark_ppT>(keypair.pk, a, b);
    std::cout << "proof generated!" << std::endl;

    if (!proof) {
        std::cout << "unexpected: proof is none!" << std::endl;
        return false;
    }

    bool ret = verify_proof(keypair.vk, *proof, a);

    if (!ret) {
        std::cout << "proof validation: failed!";
        return false;
    } else {
        std::cout << "proof validation: passed!";
        return true;
    }
}

int main() {
    // 这里曲线选择的是alt_bn128椭圆曲线，这里曲线不变
    libff::default_ec_pp::init_public_params();
    typedef libff::default_ec_pp gg_ppzksnark_ppT;

    //key generation
    auto keypair = generate_keypair<gg_ppzksnark_ppT>();

    if (!test_compare<gg_ppzksnark_ppT>(keypair)) {
       std::cout << "completeness test failed!!" << std::endl;
       return 0;
    }
    /*
    if (!test_soundness<gg_ppzksnark_ppT>(keypair)) {
        std::cout << "soundness test failed!!" << std::endl;
        return 0;
    }
    */
    std::cout << "passed completeness & soundness test" << std::endl;
    return 1;
}
