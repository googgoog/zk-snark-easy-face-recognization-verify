#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadget.hpp>
#include <iostream>

using namespace libsnark;

// template<typename FieldT>
// class add_gadget : public gadget<FieldT> {
// private:
// /* internal */
// public:
//     pb_variable<FieldT> A;
//     pb_variable<FieldT> B;
//     pb_variable<FieldT> sum;

//     add_gadget(protoboard<FieldT> &pb, const pb_variable<FieldT> &A, const pb_variable<FieldT> &B, 
//             pb_variable<FieldT> &sum, const std::string &annotation_prefix) :
//         gadget<FieldT>(pb, annotation_prefix), A(A), B(B), sum(sum) {
//         }

//     void generate_r1cs_constraints() {
//         this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
//                 {ONE},
//                 {A, B},
//                 {sum}
//         ));
//     }

//     void generate_r1cs_witness() {
//        FieldT total = FieldT::zero();
//        total += this->pb.lc_val(A) * this->pb.lc_val(B);
//        this->pb.val(sum) = total;
//     }
// };

template<typename FieldT>
class compare_gadget : public gadget<FieldT> {
    private:
    /* internal */
    public:

    /**
     * A、B分别是要比较的两个输入
     * A是阈值
     * less、less_or_eq是比较的结果    
     */
    pb_variable<FieldT> A, less, less_or_eq;
    std::shared_ptr<comparison_gadget<FieldT>> cmp;
    std::shared_ptr<inner_product_gadget<FieldT>> inner1;
    std::shared_ptr<inner_product_gadget<FieldT>> inner2;
    std::shared_ptr<inner_product_gadget<FieldT>> inner3;
    pb_variable_array<FieldT> B, C;
    pb_variable<FieldT> sum, ABCSum, sqrBC, sqrFullBC;
    pb_variable<FieldT> result1, result2, result3;

    compare_gadget(protoboard<FieldT> &pb, const std::string &annotation_prefix) :
            gadget<FieldT>(pb, annotation_prefix) {
        A.allocate(pb, FMT(annotation_prefix, "A"));
        less.allocate(pb, FMT(annotation_prefix, "less"));
        less_or_eq.allocate(pb, FMT(annotation_prefix, "less_or_eq"));
        sum.allocate(pb, FMT(annotation_prefix, "sum"));
        ABCSum.allocate(pb, FMT(annotation_prefix, "ABCSum"));
        sqrBC.allocate(pb, FMT(annotation_prefix, "sqrBC"));
        sqrFullBC.allocate(pb, FMT(annotation_prefix, "sqrFullBC"));
        result1.allocate(pb, "result1");
        result2.allocate(pb, "result2");
        result3.allocate(pb, "result3");
        B.allocate(pb, 1032, FMT(annotation_prefix, "B"));
        C.allocate(pb, 1032, FMT(annotation_prefix, "C"));

        pb.set_input_sizes(1);

        const size_t n = 64;
        inner1 = std::make_shared<inner_product_gadget<FieldT>>(pb, B, B, result1, "inner1");
        inner2 = std::make_shared<inner_product_gadget<FieldT>>(pb, C, C, result2, "inner2");
        inner3 = std::make_shared<inner_product_gadget<FieldT>>(pb, B, C, result3, "inner3");
        cmp = std::make_shared<comparison_gadget<FieldT>>(pb, n, ABCSum, sqrFullBC, less, less_or_eq, "cmp");
    }

    void generate_r1cs_constraints() {
        /**
         * #TODO：这里应该还需要有对A、B约束其大小小于等于1，大于等于0
         * 假设A是阈值，B是人脸对比结果，这样的话需要约束less_or_eq为true
         * 不过现在先不考虑
         */
        inner1->generate_r1cs_constraints();
        inner2->generate_r1cs_constraints();
        inner3->generate_r1cs_constraints();
        this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
                {result1},
                {result2},
                {sum}
        ));
        this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
                {sum},
                {A},
                {ABCSum}
        ));
        this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
                {result3},
                {result3},
                {sqrBC}
        ));
        this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(
                {sqrBC},
                {10000},
                {sqrFullBC}
        ));
        
        // 对整个比较进行比较约束
        cmp->generate_r1cs_constraints();
        // 约束less_or_eq为ture，less_or_eq * 1 = 1
        this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(less_or_eq, 1, 1));
    }

    // void generate_r1cs_witness(const int a, const int b) {
    //     this->pb.val(A) = a;
    //     this->pb.val(B) = b;

    //     cmp->generate_r1cs_witness();
    // }

    void generate_r1cs_witness(const long long a, const std::vector<int> &b, const std::vector<int> &c) {
        this->pb.val(A) = a;
        for(size_t i = 0; i < 1032; ++i) {
            this->pb.val(B[i]) = b[i];
            this->pb.val(C[i]) = c[i];
        }
        inner1->generate_r1cs_witness();
        inner2->generate_r1cs_witness();
        inner3->generate_r1cs_witness();
        FieldT total = FieldT::zero();
        total += this->pb.val(result1) * this->pb.val(result2);
        this->pb.val(sum) = total;
        this->pb.val(ABCSum) = this->pb.val(sum) * this->pb.val(A);
        this->pb.val(sqrBC) = this->pb.val(result3) * this->pb.val(result3);
        this->pb.val(sqrFullBC) = this->pb.val(sqrBC) * 10000;
        // add->generate_r1cs_witness();
        cmp->generate_r1cs_witness();
    }
    
};
