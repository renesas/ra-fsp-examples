/*
 * SPDX-FileCopyrightText: Copyright 2021-2024 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <face_detection_app/common/log_macros.h>
#include <face_detection_app/common/Model.hpp>
#include <cinttypes>
#include <memory>

arm::app::Model::Model() : m_inited(false), m_type(kTfLiteNoType) {}

/* Initialise the model */
bool arm::app::Model::Init(uint8_t* tensorArenaAddr,
                           uint32_t tensorArenaSize,
                           const uint8_t* nnModelAddr,
                           uint32_t nnModelSize,
                           tflite::MicroAllocator* allocator)
{
    /* Following tf lite micro example:
     * Map the model into a usable data structure. This doesn't involve any
     * copying or parsing, it's a very lightweight operation. */
    debug("loading model from @ 0x%p\n", nnModelAddr);
    debug("model size: %" PRIu32 " bytes.\n", nnModelSize);

    this->m_pModel = ::tflite::GetModel(nnModelAddr);

    if (this->m_pModel->version() != TFLITE_SCHEMA_VERSION) {
        printf_err("Model's schema version %" PRIu32 " is not equal "
                   "to supported version %d.",
                   this->m_pModel->version(),
                   TFLITE_SCHEMA_VERSION);
        return false;
    }

    this->m_modelAddr = nnModelAddr;
    this->m_modelSize = nnModelSize;

    /* Pull in only the operation implementations we need.
     * This relies on a complete list of all the ops needed by this graph.
     * An easier approach is to just use the AllOpsResolver, but this will
     * incur some penalty in code space for op implementations that are not
     * needed by this graph.
     * static ::tflite::ops::micro::AllOpsResolver resolver; */
    /* NOLINTNEXTLINE(runtime-global-variables) */
    debug("loading op resolver\n");

    this->EnlistOperations();

    /* Create allocator instance, if it doesn't exist */
    this->m_pAllocator = allocator;
    if (!this->m_pAllocator) {
        /* Create an allocator instance */
        info("Creating allocator using tensor arena at 0x%p\n", tensorArenaAddr);

        this->m_pAllocator = tflite::MicroAllocator::Create(tensorArenaAddr, tensorArenaSize);

        if (!this->m_pAllocator) {
            printf_err("Failed to create allocator\n");
            return false;
        }
        debug("Created new allocator @ 0x%p\n", this->m_pAllocator);
    } else {
        debug("Using existing allocator @ 0x%p\n", this->m_pAllocator);
    }

    this->m_pInterpreter = std::make_unique<tflite::MicroInterpreter>(
        this->m_pModel, this->GetOpResolver(), this->m_pAllocator);

    if (!this->m_pInterpreter) {
        printf_err("Failed to allocate interpreter\n");
        return false;
    }

    /* Allocate memory from the tensor_arena for the model's tensors. */
    info("Allocating tensors\n");
    TfLiteStatus allocate_status = this->m_pInterpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        printf_err("tensor allocation failed!\n");
        return false;
    }

    /* Get information about the memory area to use for the model's input. */
    this->m_input.resize(this->GetNumInputs());
    for (size_t inIndex = 0; inIndex < this->GetNumInputs(); inIndex++)
        this->m_input[inIndex] = this->m_pInterpreter->input(inIndex);

    this->m_output.resize(this->GetNumOutputs());
    for (size_t outIndex = 0; outIndex < this->GetNumOutputs(); outIndex++)
        this->m_output[outIndex] = this->m_pInterpreter->output(outIndex);

    if (this->m_input.empty() || this->m_output.empty()) {
        printf_err("failed to get tensors\n");
        return false;
    } else {
        this->m_type = this->m_input[0]->type; /* Input 0 should be the main input */

        /* Clear the input & output tensors */
        for (size_t inIndex = 0; inIndex < this->GetNumInputs(); inIndex++) {
            std::memset(this->m_input[inIndex]->data.data, 0, this->m_input[inIndex]->bytes);
        }
        for (size_t outIndex = 0; outIndex < this->GetNumOutputs(); outIndex++) {
            std::memset(this->m_output[outIndex]->data.data, 0, this->m_output[outIndex]->bytes);
        }

        this->LogInterpreterInfo();
    }

    this->m_inited = true;
    return true;
}

tflite::MicroAllocator* arm::app::Model::GetAllocator()
{
    if (this->IsInited()) {
        return this->m_pAllocator;
    }
    return nullptr;
}

void arm::app::Model::LogTensorInfo(TfLiteTensor* tensor)
{
    if (!tensor) {
        printf_err("Invalid tensor\n");
        assert(tensor);
        return;
    }

    debug("\ttensor is assigned to 0x%p\n", tensor);
    info("\ttensor type is %s\n", TfLiteTypeGetName(tensor->type));
    info("\ttensor occupies %zu bytes with dimensions\n", tensor->bytes);
    for (int i = 0; i < tensor->dims->size; ++i) {
        info("\t\t%d: %3d\n", i, tensor->dims->data[i]);
    }

    TfLiteQuantization quant = tensor->quantization;
    if (kTfLiteAffineQuantization == quant.type) {
        auto* quantParams = (TfLiteAffineQuantization*)quant.params;
        info("Quant dimension: %" PRIi32 "\n", quantParams->quantized_dimension);
        for (int i = 0; i < quantParams->scale->size; ++i) {
            info("Scale[%d] = %f\n", i, quantParams->scale->data[i]);
        }
        for (int i = 0; i < quantParams->zero_point->size; ++i) {
            info("ZeroPoint[%d] = %d\n", i, quantParams->zero_point->data[i]);
        }
    }
}

void arm::app::Model::LogInterpreterInfo()
{
    if (!this->m_pInterpreter) {
        printf_err("Invalid interpreter\n");
        return;
    }

    info("Model INPUT tensors: \n");
    for (auto input : this->m_input) {
        this->LogTensorInfo(input);
    }

    info("Model OUTPUT tensors: \n");
    for (auto output : this->m_output) {
        this->LogTensorInfo(output);
    }

    info("Activation buffer (a.k.a tensor arena) size used: %zu\n",
         this->m_pInterpreter->arena_used_bytes());

    /* We expect there to be only one subgraph. */
    const uint32_t nOperators = tflite::NumSubgraphOperators(this->m_pModel, 0);
    info("Number of operators: %" PRIu32 "\n", nOperators);

    const tflite::SubGraph* subgraph = this->m_pModel->subgraphs()->Get(0);

    auto* opcodes = this->m_pModel->operator_codes();

    /* For each operator, display registration information. */
    for (size_t i = 0; i < nOperators; ++i) {
        const tflite::Operator* op         = subgraph->operators()->Get(i);
        const tflite::OperatorCode* opcode = opcodes->Get(op->opcode_index());
        const TFLMRegistration* reg        = nullptr;

        tflite::GetRegistrationFromOpCode(opcode, this->GetOpResolver(), &reg);
        std::string opName;

        if (reg) {
            if (tflite::BuiltinOperator_CUSTOM == reg->builtin_code) {
                opName = std::string(reg->custom_name);
            } else {
                opName = std::string(
                    EnumNameBuiltinOperator(tflite::BuiltinOperator(reg->builtin_code)));
            }
        }
        info("\tOperator %zu: %s\n", i, opName.c_str());
    }
}

bool arm::app::Model::IsInited() const
{
    return this->m_inited;
}

bool arm::app::Model::IsDataSigned() const
{
    return this->GetType() == kTfLiteInt8;
}

bool arm::app::Model::ContainsEthosUOperator() const
{
    /* We expect there to be only one subgraph. */
    const uint32_t nOperators        = tflite::NumSubgraphOperators(this->m_pModel, 0);
    const tflite::SubGraph* subgraph = this->m_pModel->subgraphs()->Get(0);
    const auto* opcodes              = this->m_pModel->operator_codes();

    /* check for custom operators */
    for (size_t i = 0; (i < nOperators); ++i) {
        const tflite::Operator* op         = subgraph->operators()->Get(i);
        const tflite::OperatorCode* opcode = opcodes->Get(op->opcode_index());

        auto builtin_code = tflite::GetBuiltinCode(opcode);
        if ((builtin_code == tflite::BuiltinOperator_CUSTOM) &&
            (nullptr != opcode->custom_code()) &&
            ("ethos-u" == std::string(opcode->custom_code()->c_str()))) {
            return true;
        }
    }
    return false;
}

bool arm::app::Model::RunInference()
{
    bool inference_state = false;
    if (this->m_pModel && this->m_pInterpreter) {
        if (kTfLiteOk != this->m_pInterpreter->Invoke()) {
            printf_err("Invoke failed.\n");
        } else {
            inference_state = true;
        }
    } else {
        printf_err("Error: No interpreter!\n");
    }
    return inference_state;
}

TfLiteTensor* arm::app::Model::GetInputTensor(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index);
    }
    return nullptr;
}

TfLiteTensor* arm::app::Model::GetOutputTensor(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index);
    }
    return nullptr;
}

size_t arm::app::Model::GetNumInputs() const
{
    if (this->m_pModel && this->m_pInterpreter) {
        return this->m_pInterpreter->inputs_size();
    }
    return 0;
}

size_t arm::app::Model::GetNumOutputs() const
{
    if (this->m_pModel && this->m_pInterpreter) {
        return this->m_pInterpreter->outputs_size();
    }
    return 0;
}

TfLiteType arm::app::Model::GetType() const
{
    return this->m_type;
}

TfLiteIntArray* arm::app::Model::GetInputShape(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index)->dims;
    }
    return nullptr;
}

TfLiteIntArray* arm::app::Model::GetOutputShape(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index)->dims;
    }
    return nullptr;
}

bool arm::app::Model::ShowModelInfoHandler()
{
    if (!this->IsInited()) {
        printf_err("Model is not initialised! Terminating processing.\n");
        return false;
    }

    info("Model address: 0x%p", this->ModelPointer());
    info("Model size:      %" PRIu32 " bytes.", this->ModelSize());
    info("Model info:\n");
    this->LogInterpreterInfo();

    info("The model is optimised for Ethos-U NPU: %s.\n",
         this->ContainsEthosUOperator() ? "yes" : "no");

    return true;
}

const uint8_t* arm::app::Model::ModelPointer()
{
    return this->m_modelAddr;
}

uint32_t arm::app::Model::ModelSize()
{
    return this->m_modelSize;
}
