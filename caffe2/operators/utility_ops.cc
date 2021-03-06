#include "caffe2/operators/utility_ops.h"

namespace caffe2 {
namespace {

REGISTER_CPU_OPERATOR(Free, FreeOp);
REGISTER_CPU_OPERATOR(Print, PrintOp<float, CPUContext>);
REGISTER_CPU_OPERATOR(PrintInt, PrintOp<int, CPUContext>);
REGISTER_CPU_OPERATOR(Flatten, FlattenOp<CPUContext>);
REGISTER_CPU_OPERATOR(Alias, AliasOp<CPUContext>);
REGISTER_CPU_OPERATOR(ReshapeLike, ReshapeLikeOp<CPUContext>);
REGISTER_CPU_OPERATOR(Split, SplitOp<CPUContext>);
REGISTER_CPU_OPERATOR(Sum, SumOp<float, CPUContext>);
REGISTER_CPU_OPERATOR(WeightedSum, WeightedSumOp<float, CPUContext>);
REGISTER_CPU_OPERATOR(Copy, CopyOp<CPUContext, CPUContext, CPUContext>);

SHOULD_NOT_DO_GRADIENT(Free);
SHOULD_NOT_DO_GRADIENT(Print);
SHOULD_NOT_DO_GRADIENT(PrintInt);

struct GetFlattenGradient : public GetGradientDefBase {
  vector<OperatorDef>* Create(const OperatorDef& def) override {
    return SingleGradientDef(
        "ReshapeLike", "",
        vector<string>{GO(def, 0), I(def, 0)},
        vector<string>{GI(def, 0)});
  }
};
REGISTER_GRADIENT(Flatten, GetFlattenGradient);

struct GetAliasGradient : public GetGradientDefBase {
  vector<OperatorDef>* Create(const OperatorDef& def) override {
    return SingleGradientDef(
        "Alias", "",
        vector<string>{GO(def, 0)},
        vector<string>{GI(def, 0)});
  }
};
REGISTER_GRADIENT(Alias, GetAliasGradient);

SHOULD_NOT_DO_GRADIENT(ReshapeLike);

struct GetSplitGradient : public GetGradientDefBase {
  vector<OperatorDef>* Create(const OperatorDef& def) override {
    vector<string> grad_input;
    for (const string out : def.output()) {
      grad_input.push_back(GradientName(out));
    }
    return SingleGradientDef(
        "Sum", "", grad_input,
        vector<string>{GI(def, 0)});
  }
};
REGISTER_GRADIENT(Split, GetSplitGradient);

// TODO: do gradient for sum
GRADIENT_NOT_IMPLEMENTED_YET(Sum);

// TODO: Weighted sum is originally intended to be used in SGD, but in theory,
// its gradient DOES exist. Should we enable the gradient?
SHOULD_NOT_DO_GRADIENT(WeightedSum);

// TODO: Copy is a bit tricky because one need to figure out correctly where
// the input lies (e.g. for muji, which gpu). Right now I am marking it as
// not gradien ready.
SHOULD_NOT_DO_GRADIENT(Copy);

}  // namespace
}  // namespace caffe2


