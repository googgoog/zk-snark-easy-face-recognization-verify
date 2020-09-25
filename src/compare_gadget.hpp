#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <libsnark/gadgetlib1/gadget.hpp>
#include <iostream>

using namespace libsnark;

template<typename FieldT>
class compare_gadget : public gadget<FieldT> {
    private:
    /* internal */
    public:

    /**
     * A、B分别是要比较的两个输入
     * less、less_or_eq是比较的结果    
     */
    pb_variable<FieldT> A, B, less, less_or_eq;
    // const size_t n;
    std::shared_ptr<comparison_gadget<FieldT>> cmp;
   

    compare_gadget(protoboard<FieldT> &pb, const std::string &annotation_prefix) :
            gadget<FieldT>(pb, annotation_prefix) {
        A.allocate(pb, FMT(annotation_prefix, "A"));
        B.allocate(pb, FMT(annotation_prefix, "B"));
        less.allocate(pb, FMT(annotation_prefix, "less"));
        less_or_eq.allocate(pb, FMT(annotation_prefix, "less_or_eq"));
            
        pb.set_input_sizes(1);
        const size_t n = 7;

        std::cout << "n的值是:" << n << std::endl;

        cmp = std::make_shared<comparison_gadget<FieldT>>(pb, n, A, B, less, less_or_eq, "cmp");
    }

    void  generate_r1cs_constraints() {
        /**
         * #TODO：这里应该还需要有对A、B约束其大小小于等于1，大于等于0
         * 假设A是阈值，B是人脸对比结果，这样的话需要约束less_or_eq为true
         * 不过现在先不考虑
         */

        // 对整个比较进行比较约束
        cmp->generate_r1cs_constraints();
        // 约束less_or_eq为ture，less_or_eq * 1 = 1
        this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(less_or_eq, 1, 1));

 

    }

    void generate_r1cs_witness(const int a, const int b) {
        this->pb.val(A) = a;
        this->pb.val(B) = b;

        cmp->generate_r1cs_witness();
    }
    
};