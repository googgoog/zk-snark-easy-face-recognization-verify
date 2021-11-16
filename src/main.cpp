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
#include <stdlib.h>

using namespace libff;
using namespace std;

template<typename gg_ppzksnark_ppT>
void print_pk_to_file(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair, string pathToFile) {
    fstream pk(pathToFile, ios_base::out);
    pk << keypair.pk;
    pk.close();
}

template<typename gg_ppzksnark_ppT>
void print_vk_to_file(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair, string pathToFile)
{
    ofstream vk_data;
    vk_data.open(pathToFile + "vk_data");

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

    fstream vk(pathToFile + "vk_data.raw", ios_base::out);
    vk << keypair.vk;
    vk.close();
}

template<typename gg_ppzksnark_ppT>
void print_proof_to_file(r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT> proof, string pathToFile)
{
    ofstream proof_data;
    proof_data.open(pathToFile  + "proof_data");

    // gg_ppzksnark_proof 和 ppzksnark_proof的结构不相同
    G1<gg_ppzksnark_ppT> g_A(proof.g_A);
    g_A.to_affine_coordinates();
    
    G2<gg_ppzksnark_ppT> g_B(proof.g_B);
    g_B.to_affine_coordinates();

    G1<gg_ppzksnark_ppT> g_C(proof.g_C);
    g_C.to_affine_coordinates();

    proof_data << g_A.X << endl;
    proof_data << g_A.Y << endl;

    proof_data << g_B.X << endl;
    proof_data << g_B.Y << endl;

    proof_data << g_C.X << endl;
    proof_data << g_C.Y << endl;

    proof_data.close();

    fstream proof_raw(pathToFile + "proof_data.raw", ios_base::out);
    proof_raw << proof;
    proof_raw.close();
}

template<typename gg_ppzksnark_ppT>
bool test_compare(r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> keypair) {
    cout << "trying to generate proof..." << endl;    
    int a = 6573;
    // int b = 100;
    std::vector<int> b = {0, 128, 6, 68, 0, 0, 96, 65, 85, 10, 1, 61, 58, 22, 6, 67, 87, 78, 88, 67, 73, 37, 7, 59, 107, 91, 118, 61, 90, 90, 97, 68, 79, 29, 1, 62, 21, 59, 5, 67, 88, 33, 99, 67, 102, 7, 116, 61, 94, 46, 120, 61, 30, 6, 16, 60, 73, 18, 120, 60, 17, 122, 60, 67, 0, 25, 118, 61, 6, 27, 14, 60, 79, 8, 38, 61, 39, 61, 78, 67, 55, 119, 46, 68, 49, 8, 12, 61, 112, 120, 30, 61, 23, 4, 33, 66, 2, 123, 62, 60, 40, 122, 120, 61, 90, 95, 64, 61, 13, 105, 71, 68, 49, 114, 120, 61, 41, 54, 95, 61, 9, 75, 14, 60, 56, 95, 6, 67, 53, 70, 6, 62, 4, 79, 57, 60, 72, 23, 26, 67, 6, 28, 43, 67, 87, 66, 37, 68, 19, 86, 107, 67, 77, 29, 19, 61, 19, 119, 42, 62, 72, 115, 47, 61, 80, 90, 82, 67, 11, 2, 49, 61, 122, 44, 1, 62, 125, 54, 72, 61, 8, 116, 14, 68, 115, 125, 73, 67, 83, 41, 34, 68, 84, 3, 11, 66, 61, 127, 47, 61, 63, 59, 29, 67, 57, 105, 112, 61, 114, 5, 128, 67, 31, 32, 117, 67, 48, 94, 57, 61, 83, 61, 75, 68, 79, 31, 54, 61, 121, 76, 73, 60, 100, 76, 3, 67, 80, 46, 49, 67, 72, 86, 97, 61, 47, 93, 102, 67, 114, 16, 57, 68, 34, 82, 47, 68, 87, 110, 99, 60, 81, 110, 37, 60, 48, 98, 101, 61, 116, 101, 63, 67, 87, 104, 115, 61, 17, 115, 72, 61, 21, 93, 109, 60, 61, 59, 67, 61, 59, 82, 63, 61, 61, 16, 62, 61, 73, 118, 112, 61, 113, 4, 55, 61, 108, 110, 81, 67, 107, 51, 98, 69, 92, 28, 47, 67, 119, 34, 60, 67, 8, 120, 112, 59, 119, 84, 22, 67, 7, 24, 10, 61, 56, 31, 11, 67, 111, 45, 48, 67, 39, 31, 104, 67, 68, 60, 6, 61, 49, 5, 38, 67, 60, 88, 103, 67, 89, 73, 102, 61, 58, 42, 101, 67, 34, 44, 95, 60, 95, 43, 122, 67, 38, 98, 116, 67, 87, 117, 107, 67, 29, 118, 77, 61, 124, 74, 124, 68, 19, 36, 35, 60, 21, 101, 49, 68, 75, 28, 9, 67, 53, 100, 94, 67, 103, 56, 66, 68, 47, 119, 2, 60, 2, 80, 86, 67, 15, 62, 85, 68, 52, 46, 75, 61, 62, 30, 85, 60, 105, 17, 29, 60, 6, 109, 80, 67, 6, 80, 114, 61, 63, 77, 37, 68, 113, 33, 124, 68, 94, 106, 87, 60, 91, 77, 106, 67, 45, 24, 42, 68, 121, 105, 47, 67, 67, 53, 121, 68, 63, 123, 21, 67, 89, 128, 97, 68, 38, 9, 126, 68, 38, 76, 125, 68, 57, 59, 74, 61, 115, 23, 123, 59, 37, 47, 57, 58, 105, 101, 58, 68, 66, 40, 113, 61, 63, 29, 116, 61, 24, 36, 81, 67, 2, 95, 57, 67, 36, 97, 47, 60, 9, 7, 127, 67, 93, 81, 85, 67, 77, 9, 91, 67, 16, 67, 18, 61, 105, 103, 128, 67, 79, 104, 38, 61, 9, 33, 9, 66, 62, 99, 14, 68, 88, 106, 53, 68, 111, 23, 81, 70, 104, 118, 117, 68, 82, 49, 66, 60, 67, 50, 24, 69, 87, 35, 63, 60, 118, 78, 43, 68, 43, 108, 88, 67, 114, 76, 127, 68, 68, 75, 50, 67, 22, 81, 42, 61, 1, 90, 32, 60, 40, 23, 8, 68, 27, 69, 20, 60, 72, 73, 109, 60, 115, 117, 15, 67, 73, 105, 80, 61, 42, 22, 13, 67, 8, 62, 30, 69, 31, 43, 37, 61, 104, 26, 41, 67, 90, 103, 77, 61, 103, 102, 4, 62, 117, 60, 77, 67, 68, 115, 119, 67, 120, 24, 77, 68, 60, 41, 64, 67, 117, 76, 88, 68, 42, 67, 104, 60, 76, 11, 76, 61, 79, 17, 8, 60, 104, 126, 43, 68, 37, 82, 14, 68, 77, 104, 39, 70, 45, 1, 41, 68, 67, 59, 123, 61, 51, 23, 7, 60, 126, 124, 125, 67, 86, 114, 52, 67, 50, 115, 99, 60, 96, 99, 4, 68, 1, 9, 112, 59, 117, 58, 24, 60, 95, 89, 76, 67, 47, 5, 15, 61, 6, 71, 116, 69, 68, 2, 28, 61, 126, 75, 14, 60, 11, 83, 22, 61, 87, 61, 10, 62, 26, 124, 91, 67, 104, 86, 88, 67, 26, 20, 22, 61, 103, 21, 41, 59, 56, 92, 83, 67, 8, 128, 45, 60, 33, 53, 97, 67, 118, 90, 107, 61, 45, 118, 94, 59, 92, 49, 100, 67, 36, 106, 120, 61, 91, 51, 21, 66, 120, 69, 33, 67, 69, 1, 94, 61, 97, 103, 66, 61, 14, 8, 102, 61, 87, 102, 51, 61, 76, 17, 85, 61, 81, 50, 119, 61, 58, 52, 9, 66, 93, 52, 18, 60, 53, 116, 93, 69, 13, 17, 6, 60, 26, 25, 78, 68, 18, 59, 120, 68, 84, 113, 25, 61, 57, 120, 19, 62, 118, 18, 26, 61, 7, 29, 51, 61, 120, 111, 65, 68, 2, 39, 13, 59, 49, 27, 121, 59, 100, 85, 19, 67, 40, 19, 9, 67, 25, 119, 37, 61, 95, 59, 78, 61, 81, 91, 53, 67, 90, 50, 123, 60, 127, 108, 43, 61, 39, 94, 118, 61, 82, 40, 45, 61, 1, 58, 85, 67, 27, 69, 47, 61, 13, 78, 117, 60, 79, 88, 122, 61, 125, 16, 52, 59, 34, 72, 56, 61, 96, 98, 64, 68, 117, 24, 103, 67, 23, 86, 102, 61, 85, 46, 12, 61, 29, 48, 48, 59, 8, 103, 42, 61, 71, 74, 44, 60, 4, 54, 45, 60, 15, 108, 89, 61, 107, 77, 115, 61, 5, 46, 67, 61, 123, 64, 40, 59, 29, 33, 37, 61, 75, 102, 112, 67, 56, 17, 11, 61, 67, 33, 106, 61, 109, 116, 51, 60, 101, 84, 80, 61, 49, 99, 50, 68, 6, 117, 50, 67, 80, 124, 59, 61, 126, 22, 54, 68, 91, 18, 25, 60};
    std::vector<int> c = {0, 128, 6, 68, 0, 0, 96, 65, 40, 119, 11, 61, 106, 47, 0, 66, 69, 43, 58, 59, 41, 64, 128, 61, 37, 109, 68, 60, 4, 126, 74, 61, 17, 37, 86, 61, 69, 45, 106, 67, 17, 8, 118, 67, 44, 91, 86, 69, 113, 115, 20, 68, 113, 30, 50, 67, 28, 65, 122, 67, 47, 35, 64, 67, 11, 58, 21, 68, 94, 20, 80, 58, 64, 17, 55, 61, 109, 7, 83, 60, 17, 108, 57, 67, 114, 34, 46, 67, 114, 48, 35, 67, 72, 109, 59, 60, 50, 116, 11, 61, 38, 127, 1, 68, 29, 75, 91, 67, 84, 99, 118, 67, 58, 113, 7, 68, 38, 101, 121, 61, 39, 126, 95, 68, 78, 50, 8, 66, 102, 34, 67, 61, 7, 98, 29, 66, 93, 99, 82, 67, 72, 38, 59, 67, 21, 51, 117, 60, 3, 22, 126, 67, 17, 112, 22, 69, 41, 53, 73, 61, 54, 51, 1, 67, 36, 18, 124, 60, 117, 98, 88, 61, 105, 19, 115, 61, 106, 67, 59, 61, 82, 74, 42, 67, 119, 59, 28, 67, 71, 12, 76, 61, 1, 127, 74, 60, 97, 76, 19, 61, 60, 41, 76, 67, 59, 57, 10, 62, 91, 120, 45, 61, 45, 122, 116, 61, 5, 103, 16, 61, 92, 112, 15, 67, 119, 14, 63, 61, 23, 19, 43, 61, 102, 7, 37, 67, 60, 90, 8, 67, 63, 96, 116, 60, 1, 100, 16, 66, 11, 74, 62, 67, 120, 59, 18, 67, 70, 4, 110, 68, 71, 82, 39, 61, 97, 112, 51, 67, 34, 28, 20, 66, 122, 120, 92, 68, 78, 122, 7, 61, 13, 24, 28, 69, 115, 7, 116, 60, 17, 18, 74, 61, 55, 40, 37, 61, 102, 127, 50, 67, 21, 1, 17, 62, 121, 51, 1, 60, 30, 123, 87, 69, 118, 65, 31, 66, 117, 35, 26, 60, 126, 11, 63, 58, 61, 121, 33, 67, 68, 81, 57, 61, 42, 43, 36, 67, 65, 23, 120, 67, 6, 15, 81, 60, 104, 41, 31, 60, 103, 112, 25, 67, 13, 35, 93, 67, 116, 121, 93, 68, 63, 122, 89, 60, 71, 101, 66, 61, 12, 115, 36, 67, 71, 4, 86, 67, 74, 84, 34, 67, 56, 118, 10, 68, 86, 101, 68, 61, 52, 66, 109, 67, 5, 83, 119, 68, 106, 88, 20, 67, 112, 116, 98, 67, 102, 93, 3, 67, 47, 84, 51, 67, 50, 57, 44, 66, 89, 41, 5, 60, 112, 16, 23, 70, 85, 32, 77, 67, 105, 18, 50, 67, 70, 107, 21, 66, 92, 24, 5, 62, 25, 5, 70, 67, 122, 109, 84, 61, 13, 100, 4, 61, 0, 80, 118, 68, 61, 76, 11, 67, 27, 61, 34, 68, 77, 40, 106, 60, 82, 67, 90, 67, 102, 45, 40, 59, 89, 71, 112, 67, 120, 94, 106, 67, 119, 56, 71, 60, 9, 79, 9, 61, 102, 66, 73, 67, 57, 3, 93, 61, 77, 1, 77, 61, 59, 117, 56, 60, 30, 56, 22, 59, 114, 15, 98, 68, 123, 52, 25, 61, 1, 7, 17, 67, 61, 110, 68, 67, 97, 108, 75, 60, 22, 41, 36, 70, 29, 115, 11, 59, 103, 35, 88, 60, 70, 6, 92, 67, 17, 90, 55, 60, 39, 127, 94, 68, 44, 124, 84, 61, 6, 47, 128, 69, 89, 18, 79, 67, 5, 128, 64, 68, 20, 95, 43, 61, 75, 86, 28, 67, 77, 42, 116, 67, 12, 118, 91, 60, 2, 112, 112, 67, 59, 46, 43, 61, 126, 95, 79, 68, 88, 100, 3, 61, 33, 65, 69, 61, 14, 89, 55, 68, 76, 40, 126, 67, 38, 38, 100, 61, 122, 43, 114, 67, 93, 46, 55, 68, 36, 40, 95, 68, 122, 128, 20, 67, 124, 68, 2, 67, 94, 69, 39, 60, 107, 20, 21, 59, 100, 12, 119, 69, 104, 42, 52, 61, 79, 63, 42, 69, 68, 28, 21, 67, 64, 104, 22, 60, 22, 41, 9, 60, 14, 26, 106, 61, 95, 82, 83, 60, 123, 73, 42, 60, 7, 74, 30, 61, 59, 100, 122, 67, 66, 111, 34, 62, 26, 40, 8, 61, 67, 101, 108, 61, 52, 48, 29, 67, 54, 112, 111, 61, 84, 41, 37, 67, 71, 123, 124, 61, 126, 80, 65, 68, 76, 68, 124, 67, 125, 79, 93, 61, 89, 7, 107, 60, 38, 8, 36, 61, 58, 32, 79, 61, 107, 24, 25, 68, 125, 19, 99, 61, 18, 124, 89, 69, 84, 108, 87, 67, 1, 35, 90, 60, 122, 0, 102, 61, 13, 92, 5, 67, 54, 84, 94, 67, 62, 42, 34, 67, 44, 99, 119, 61, 16, 71, 33, 69, 14, 11, 43, 68, 18, 73, 116, 61, 86, 98, 109, 67, 25, 59, 72, 66, 9, 101, 61, 60, 20, 113, 124, 60, 11, 105, 86, 61, 57, 20, 97, 61, 6, 53, 102, 67, 100, 33, 14, 61, 9, 23, 112, 67, 18, 97, 51, 67, 6, 84, 76, 60, 67, 50, 33, 60, 13, 126, 3, 68, 94, 61, 77, 61, 10, 105, 29, 61, 107, 38, 108, 60, 112, 70, 42, 61, 56, 6, 112, 61, 110, 102, 91, 68, 48, 63, 94, 61, 52, 58, 6, 61, 95, 83, 22, 67, 18, 100, 21, 60, 84, 1, 44, 60, 49, 88, 68, 62, 108, 105, 44, 67, 17, 3, 31, 60, 71, 102, 43, 61, 81, 127, 72, 67, 74, 62, 75, 60, 124, 29, 39, 60, 10, 9, 60, 59, 23, 101, 88, 60, 105, 111, 105, 61, 31, 6, 63, 61, 105, 84, 25, 67, 59, 24, 119, 61, 90, 103, 34, 67, 35, 84, 75, 70, 88, 81, 83, 61, 69, 40, 27, 61, 3, 37, 1, 62, 100, 82, 23, 68, 26, 40, 47, 67, 125, 73, 72, 60, 94, 95, 50, 61, 114, 112, 76, 61, 59, 24, 80, 67, 107, 73, 32, 61, 90, 124, 79, 67, 83, 20, 115, 60, 117, 22, 16, 61, 128, 32, 61, 60, 74, 60, 6, 69, 100, 32, 62, 60, 55, 7, 122, 67, 101, 51, 88, 61, 43, 4, 95, 61, 128, 52, 72, 67};

    // std::vector<int> _inputs_a = {1, 3, 2, 4};
    // std::vector<int> _inputs_b = {2, 4, 1, 3};
    // std::vector<int> _inputs_c = {3, 1, 4, 2};
    // std::vector<int> _inputs_d = {4, 2, 3, 1};

    auto proof = generate_proof<gg_ppzksnark_ppT>(keypair.pk, a, b, c);
    // auto proof = generate_proof<gg_ppzksnark_ppT>(keypair.pk, _inputs_a,
    //                                            _inputs_b, _inputs_c, _inputs_d);

    cout << "proof generated!" << endl;

    if (!proof) {
        cout << "unexpected: proof is none!" << endl;
        return false;
    }

    // 生成证明文件
    // print_proof_to_file<gg_ppzksnark_ppT>(*proof, "../proof_data");

    bool ret = verify_proof(keypair.vk, *proof, a);
    // bool ret = verify_proof(keypair.vk, *proof, 2, 2, 3, 3, 2, 3);

    if (!ret) {
        cout << "proof validation: failed!";
        return false;
    } else {
        cout << "proof validation: passed!";
        return true;
    }
}



int main(int argc, char* argv[]) {
    // 这里曲线选择的是alt_bn128椭圆曲线，这里曲线不变
    libff::default_ec_pp::init_public_params();
    typedef libff::default_ec_pp gg_ppzksnark_ppT;

    auto keypair = generate_keypair<gg_ppzksnark_ppT>();

    if (!test_compare<gg_ppzksnark_ppT>(keypair)) {
       std::cout << "completeness test failed!!" << std::endl;
       return 0;
    }

    // if (string(argv[1]) == "setup") {
    //     /**
    //      * @param argv[2]:密钥路径
    //      * */

    //     //key generation
    //     auto keypair = generate_keypair<gg_ppzksnark_ppT>();

    //     string pk_path = string(argv[2]) + "pk_data.raw";
    //     print_pk_to_file<gg_ppzksnark_ppT>(keypair, pk_path);

    //     string vk_path = string(argv[2]);
    //     print_vk_to_file<gg_ppzksnark_ppT>(keypair, vk_path);

    // } else if (string(argv[1]) == "prove") {
    //     /**
    //      * @param argv[2]:pk.raw路径
    //      * @param argv[3]:primary input(阈值)
    //      * @param argv[4]:auxilary input(人脸比较值)
    //      * @param argv[5]:proof_data路径
    //      * */

    //     //load pk
    //     fstream f_pk(string(argv[2]), ios_base::in);
    //     r1cs_gg_ppzksnark_proving_key<gg_ppzksnark_ppT> pk;
    //     f_pk >> pk;
    //     f_pk.close();

    //     cout << "trying to generate proof..." << endl;
        
    //     int threshold = atoi(argv[3]);
    //     auto proof = generate_proof<gg_ppzksnark_ppT>(pk, threshold, atoi(argv[4]));

    //     if (proof != boost::none) {
    //         cout << "Proof generated!" << endl;
    //     }

    //     string proof_data_path = string(argv[5]);
    //     print_proof_to_file<gg_ppzksnark_ppT>(*proof, proof_data_path);

    // } else if (string(argv[1]) == "verify") {
    //     /**
    //      * @param argv[2]:proof.raw路径
    //      * @param argv[3]:vk.raw路径
    //      * @param argv[4]:primary input(阈值)
    //      * */

    //     //load proof    
    //     std::fstream pr(string(argv[2]), std::ios_base::in);
    //     r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT> proof;
    //     pr >> proof;
    //     pr.close();

	//     //load vk
    //     std::fstream vkf(string(argv[3]), std::ios_base::in);
    //     r1cs_gg_ppzksnark_verification_key<gg_ppzksnark_ppT> vk;
    //     vkf >> vk;
    //     vkf.close();

    //     bool ret = verify_proof<gg_ppzksnark_ppT>(vk, proof, atoi(argv[4]));
    //     if (ret) {
    //         std::cout << "Verification pass!" << std::endl;
    //     } else {
    //         std::cout << "Verification failed!" << std::endl;
    //     }

    // }

    return 0;
}
