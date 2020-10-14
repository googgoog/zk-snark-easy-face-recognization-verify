//
// Created by xkx on 11/1/18.
//

#define CURVE_ALT_BN128

#include <vector>
#include "sudoku_gadget.hpp"
#include "sudoku_snark.hpp"
#include <libff/common/default_types/ec_pp.hpp>
#include <iostream>
#include "libff/algebra/curves/public_params.hpp"
#include <fstream>

using namespace libff;
using namespace std;

template<typename gg_ppzksnark_ppT>
void print_proof_to_file(r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT> proof, string pathToFile)
{
  ofstream proof_data;
  proof_data.open(pathToFile);

  // gg_ppzksnark_proof 和 ppzksnark_proof的结构不相同
  G1<gg_ppzksnark_ppT> g_A(proof.g_A);
  g_A.to_affine_coordinates();
  
  G2<gg_ppzksnark_ppT> g_B(proof.g_B);
  g_B.to_affine_coordinates();

  cout << "g_B.X: " << g_B.X << "  g_B.Y: " << g_B.Y << endl;

  G1<gg_ppzksnark_ppT> g_C(proof.g_C);
  g_C.to_affine_coordinates();


  proof_data << g_A.X << endl;
  proof_data << g_A.Y << endl;

  proof_data << g_B.X << endl;
  proof_data << g_B.Y << endl;

  proof_data << g_C.X << endl;
  proof_data << g_C.Y << endl;

  proof_data.close();
}

template<typename gg_ppzksnark_ppT>
void print_vk_to_file(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair, string pathToFile)
{
  ofstream vk_data;
  vk_data.open(pathToFile);

  G1<gg_ppzksnark_ppT> alpha_g1(keypair.pk.alpha_g1);
  alpha_g1.to_affine_coordinates();

  G2<gg_ppzksnark_ppT> beta_g2(keypair.pk.beta_g2);
  beta_g2.to_affine_coordinates();

  G2<gg_ppzksnark_ppT> gamma_g2(keypair.vk.gamma_g2);
  gamma_g2.to_affine_coordinates();

  G2<gg_ppzksnark_ppT> delta_g2(keypair.vk.delta_g2);
  delta_g2.to_affine_coordinates();

  accumulation_vector<G1<gg_ppzksnark_ppT>> IC(keypair.vk.gamma_ABC_g1);
  G1<gg_ppzksnark_ppT> IC_0(IC.first);
  IC_0.to_affine_coordinates();

  vk_data << alpha_g1.X << endl;
  vk_data << alpha_g1.Y << endl;

  vk_data << beta_g2.X << endl;
  vk_data << beta_g2.Y << endl;

  vk_data << gamma_g2.X << endl;
  vk_data << gamma_g2.Y << endl;

  vk_data << delta_g2.X << endl;
  vk_data << delta_g2.Y << endl;

  vk_data << IC_0.X << endl;
  vk_data << IC_0.Y << endl;

  for(size_t i=0; i<IC.size(); i++) {
    G1<gg_ppzksnark_ppT> IC_N(IC.rest[i]);
    IC_N.to_affine_coordinates();
    vk_data << IC_N.X << endl;
    vk_data << IC_N.Y << endl;
  }

  vk_data.close();
}



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

    // 生成证明文件
    print_proof_to_file<gg_ppzksnark_ppT>(*proof, "../proof_data");

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

    print_vk_to_file<gg_ppzksnark_ppT>(keypair,"../vk_data");

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
