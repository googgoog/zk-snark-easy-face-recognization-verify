//
// Created by xkx on 11/4/18.
//

#ifndef _SUDOKU_SNARK_HPP
#define _SUDOKU_SNARK_HPP

#include "sudoku_gadget.hpp"
#include <boost/optional.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

// TODO:后面要删除
#include "compare_gadget.hpp"

using namespace libsnark;

template<typename gg_ppzksnark_ppT>
r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> generate_keypair();

template<typename gg_ppzksnark_ppT>
boost::optional<r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT>> generate_proof(r1cs_gg_ppzksnark_proving_key<gg_ppzksnark_ppT> proving_key,
                                                   std::vector<int> _inputs_a, std::vector<int> _inputs_b,
                                                   std::vector<int> _inputs_c, std::vector<int> _inputs_d);

template<typename gg_ppzksnark_ppT>
bool verify_proof(r1cs_gg_ppzksnark_verification_key<gg_ppzksnark_ppT> verification_key,
                  r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT> proof,
                  int a21, int b11, int b22,
                  int c11, int c22, int d21);

#include "sudoku_snark.cpp"

#endif
