// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <array>
#include <vector>
#include <catch2/catch_test_macros.hpp>

#include "shader_recompiler/frontend/ampere/lower_to_maxwell.h"
#include "shader_recompiler/frontend/ir/program.h"
#include "shader_recompiler/frontend/maxwell/translate/translate.h"
#include "shader_recompiler/ir_opt/passes.h"

namespace {
using Shader::Ampere::Instruction;
using Shader::Ampere::LowerToMaxwell;

constexpr Instruction EXIT{{0x0000794d, 0, 0x03800000, 0}};

class LoweredEnvironment final : public Shader::Environment {
public:
    explicit LoweredEnvironment(std::span<const u64> instructions_) : instructions{instructions_} {}

    u64 ReadInstruction(u32 address) override {
        for (u32 i = 0; i < instructions.size(); ++i) {
            if (address == 0x50 + i * 8 + ((i + 1) / 3) * 8) return instructions[i];
        }
        return 0;
    }
    bool HasNativeInstructions() const override { return true; }
    u32 ReadCbufValue(u32, u32) override { return 0; }
    Shader::TextureType ReadTextureType(u32) override { return Shader::TextureType::Color2D; }
    Shader::TexturePixelFormat ReadTexturePixelFormat(u32) override {
        return Shader::TexturePixelFormat::R32_UINT;
    }
    bool IsTexturePixelFormatInteger(u32) override { return true; }
    u32 TextureBoundBuffer() const override { return 0; }
    u32 ReadViewportTransformState() override { return 1; }
    u32 LocalMemorySize() const override { return 0; }
    u32 SharedMemorySize() const override { return 0; }
    std::array<u32, 3> WorkgroupSize() const override { return {64, 1, 1}; }
    bool HasHLEMacroState() const override { return false; }
    std::optional<Shader::ReplaceConstant> GetReplaceConstBuffer(u32, u32) override {
        return std::nullopt;
    }
    void Dump(u64, u64) override {}

private:
    std::span<const u64> instructions;
};

TEST_CASE("Shadowed registers keep their values across instructions that borrow their quartet",
          "[video_core][ampere]") {
    // R228 (UR12's mirror) and R249 (scratch) are guest registers here; each instruction that
    // touches one borrows R176-R179, which the guest also uses.
    const std::array<Instruction, 7> code{{
        {{0xffec7981, 0x00002004, 0x0c1e8b00, 0x000f2200}}, // LDG.E.64 R236, desc[UR4]
        {{0x00b07802, 0x5678, 0x00000f00, 0}},              // MOV R176, 0x5678
        {{0x00e47802, 0x1234, 0x00000f00, 0}},              // MOV R228, 0x1234
        {{0x1bf97221, 0x00000028, 0x00010000, 0x000fe200}}, // FADD R249, R27, R40
        {{0x02007386, 0x000000e4, 0x0010a900, 0}},          // STG.E [R2.64], R228
        {{0x02007386, 0x000000b0, 0x0010a900, 0}},          // STG.E [R2.64], R176
        EXIT,
    }};
    const auto lowered = LowerToMaxwell(code);
    INFO(lowered.error);
    REQUIRE(static_cast<bool>(lowered));

    LoweredEnvironment env{lowered.instructions};
    Shader::ObjectPool<Shader::IR::Inst> pool;
    Shader::IR::Block block{pool};
    Shader::IR::Program program;
    program.blocks = {&block};
    program.post_order_blocks = {&block};
    const u32 last = static_cast<u32>(lowered.instructions.size() - 1);
    Shader::Maxwell::Translate(env, &block, 0x50, 0x50 + last * 8 + ((last + 1) / 3) * 8);
    Shader::Optimization::SsaRewritePass(program);
    Shader::Optimization::ConstantPropagationPass(env, program);
    std::vector<u32> stored;
    for (const Shader::IR::Inst& inst : block) {
        if (inst.GetOpcode() != Shader::IR::Opcode::WriteGlobal32) continue;
        REQUIRE(inst.Arg(1).IsImmediate());
        stored.push_back(inst.Arg(1).U32());
    }
    CHECK(stored == std::vector<u32>{0x1234, 0x5678});
}
} // namespace
