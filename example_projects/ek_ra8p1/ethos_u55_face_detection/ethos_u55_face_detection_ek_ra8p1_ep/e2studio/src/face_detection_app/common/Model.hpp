/*
 * SPDX-FileCopyrightText: Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
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
#ifndef MODEL_HPP
#define MODEL_HPP

#include <face_detection_app/common/TensorFlowLiteMicro.hpp>
#include <cstdint>

namespace arm {
namespace app {

    /**
     * @brief   NN model class wrapping the underlying TensorFlow-Lite-Micro API.
     */
    class Model {
    public:
        /** @brief Constructor. */
        Model();

        /** @brief Destructor. */
        virtual ~Model() = default;

        /** @brief  Gets the pointer to the model's input tensor at given input index. */
        TfLiteTensor* GetInputTensor(size_t index) const;

        /** @brief  Gets the pointer to the model's output tensor at given output index. */
        TfLiteTensor* GetOutputTensor(size_t index) const;

        /** @brief  Gets the model's data type. */
        TfLiteType GetType() const;

        /** @brief  Gets the pointer to the model's input shape. */
        TfLiteIntArray* GetInputShape(size_t index) const;

        /** @brief  Gets the pointer to the model's output shape at given output index. */
        TfLiteIntArray* GetOutputShape(size_t index) const;

        /** @brief  Gets the number of input tensors the model has. */
        size_t GetNumInputs() const;

        /** @brief  Gets the number of output tensors the model has. */
        size_t GetNumOutputs() const;

        /** @brief  Logs the tensor information to stdout. */
        void LogTensorInfo(TfLiteTensor* tensor);

        /** @brief  Logs the interpreter information to stdout. */
        void LogInterpreterInfo();

        /** @brief      Initialise the model class object.
         *  @param[in]  tensorArenaAddress  Pointer to the tensor arena buffer.
         *  @param[in]  tensorArenaAddress  Size of the tensor arena buffer in bytes.
         *  @param[in]  nnModelAddr         Pointer to the model.
         *  @param[in]  nnModelSize         Size of the model in bytes, if known.
         *  @param[in]  allocator   Optional: a pre-initialised micro allocator pointer,
         *                          if available. If supplied, this allocator will be used
         *                          to create the interpreter instance.
         *  @return     true if initialisation succeeds, false otherwise.
        **/
        bool Init(uint8_t* tensorArenaAddr,
                  uint32_t tensorArenaSize,
                  const uint8_t* nnModelAddr,
                  uint32_t nnModelSize,
                  tflite::MicroAllocator* allocator = nullptr);

        /**
         * @brief       Gets the allocator pointer for this instance.
         * @return      Pointer to a tflite::MicroAllocator object, if
         *              available; nullptr otherwise.
         **/
        tflite::MicroAllocator* GetAllocator();

        /** @brief  Checks if this object has been initialised. */
        bool IsInited() const;

        /** @brief  Checks if the model uses signed data. */
        bool IsDataSigned() const;

        /** @brief Checks if the model uses Ethos-U operator */
        bool ContainsEthosUOperator() const;

        /** @brief  Runs the inference (invokes the interpreter). */
        virtual bool RunInference();

        /** @brief   Model information handler common to all models.
         *  @return  true or false based on execution success.
         **/
        bool ShowModelInfoHandler();

        /** @brief   Gets a pointer to the tensor arena. */
        uint8_t* GetTensorArena();

    protected:
        /** @brief      Gets the pointer to the NN model data array.
         *  @return     Pointer of uint8_t type.
         **/
        const uint8_t* ModelPointer();

        /** @brief      Gets the model size.
         *  @return     size_t, size in bytes.
         **/
        uint32_t ModelSize();

        /**
         * @brief       Gets the op resolver for the model instance.
         * @return      const reference to a tflite::MicroOpResolver object.
         **/
        virtual const tflite::MicroOpResolver& GetOpResolver() = 0;

        /**
         * @brief       Add all the operators required for the given model.
         *              Implementation of this should come from the use case.
         * @return      true is ops are successfully added, false otherwise.
         **/
        virtual bool EnlistOperations() = 0;

        /** @brief   Gets the total size of tensor arena available for use. */
        size_t GetActivationBufferSize();

    private:
        const tflite::Model* m_pModel{nullptr};            /* Tflite model pointer. */
        std::unique_ptr<tflite::MicroInterpreter> m_pInterpreter{nullptr}; /* Tflite interpreter. */
        tflite::MicroAllocator* m_pAllocator{nullptr};     /* Tflite micro allocator. */
        bool m_inited{false};                              /* Indicates whether this object has been initialised. */
        const uint8_t* m_modelAddr{nullptr};               /* Model address */
        uint32_t m_modelSize{0};                           /* Model size */

        std::vector<TfLiteTensor*> m_input{};              /* Model's input tensor pointers. */
        std::vector<TfLiteTensor*> m_output{};             /* Model's output tensor pointers. */
        TfLiteType m_type{kTfLiteNoType};                  /* Model's data type. */
    };

} /* namespace app */
} /* namespace arm */

#endif /* MODEL_HPP */
