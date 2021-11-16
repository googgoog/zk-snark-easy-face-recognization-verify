//
// Created by xkx on 11/5/18.
//
#ifndef _SUDOKU_SNARK_CPP
#define _SUDOKU_SNARK_CPP

#define CURVE_ALT_BN128

#include "sudoku_snark.hpp"
#include <libff/common/default_types/ec_pp.hpp>
// #include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>

template<typename gg_ppzksnark_ppT>
r1cs_gg_ppzksnark_keypair<gg_ppzksnark_ppT> generate_keypair()
{
    // FieldT在这里代表有限域的元素表示法
    typedef libff::Fr<gg_ppzksnark_ppT> FieldT;

    // protoboard是r1cs_constraint_system之上的一层封装
    protoboard<FieldT> pb;

    // sudoku_gadget本质上是1个validateInput_gadget、3个checkEquality_gadget、
    // 16个数独结果（包括数独已知部分的和数独的解）以及1个0（这个0不知道作用）
    // pb是在这里传入的
    // sudoku_gadget<FieldT> g(pb, "sudoku_gadget");

    // TODO:后面要删除
    compare_gadget<FieldT> g(pb, "compare_gadget");

    // 这里会对输入、每行、每列和每格生成相应约束
    g.generate_r1cs_constraints();
    r1cs_constraint_system<FieldT> cs = pb.get_constraint_system();

#ifdef _DEBUG
    std::cout << "Number of R1CS constraints: " << cs.num_constraints() << std::endl;
#endif
    // 这里使用的是ppzksnark证明系统，也就是PGHR13算法
    return r1cs_gg_ppzksnark_generator<gg_ppzksnark_ppT>(cs);
}

template<typename gg_ppzksnark_ppT>
boost::optional<r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT>> generate_proof(r1cs_gg_ppzksnark_proving_key<gg_ppzksnark_ppT> proving_key,
                                                                        // int _inputs_a, int _inputs_b)
                                                                        int _inputs_a, std::vector<int> _inputs_b, std::vector<int> _inputs_c)
                                                                    //    std::vector<int> _inputs_a, std::vector<int> _inputs_b,
                                                                    // std::vector<int> _inputs_c, std::vector<int> _inputs_d)
{
    typedef libff::Fr<gg_ppzksnark_ppT> FieldT;

    protoboard<FieldT> pb;
    // sudoku_gadget<FieldT> g(pb, "sudoku_gadget");
    // TODO:后面要删除
    compare_gadget<FieldT> g(pb, "compare_gadget");

    g.generate_r1cs_constraints();
    g.generate_r1cs_witness(_inputs_a, _inputs_b, _inputs_c);
    //  g.generate_r1cs_witness(_inputs_a, _inputs_b, _inputs_c, _inputs_d);

    if (!pb.is_satisfied()) {
        std::cout << "pb is not satisfied" << std::endl;
        return boost::none;
    }

    return r1cs_gg_ppzksnark_prover<gg_ppzksnark_ppT>(proving_key, pb.primary_input(), pb.auxiliary_input());
}

template<typename gg_ppzksnark_ppT>
bool verify_proof(r1cs_gg_ppzksnark_verification_key<gg_ppzksnark_ppT> verification_key,
                  r1cs_gg_ppzksnark_proof<gg_ppzksnark_ppT> proof,
                  int a)
                //   int a21, int b11, int b22,
                //   int c11, int c22, int d21)
{
    typedef libff::Fr<gg_ppzksnark_ppT> FieldT;

    // const r1cs_primary_input<FieldT> input = {FieldT(a21), FieldT(b11), FieldT(b22),
    //                                           FieldT(c11), FieldT(c22), FieldT(d21)};

    // TODO：后面要删除,a是已知的阈值
    // const r1cs_primary_input<FieldT> input = {FieldT(a), FieldT(b)};
    const r1cs_primary_input<FieldT> input = {FieldT(a)};

    return r1cs_gg_ppzksnark_verifier_strong_IC<gg_ppzksnark_ppT>(verification_key, input, proof);
}

#endif
