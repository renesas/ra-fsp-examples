/*
 * SPDX-FileCopyrightText: Copyright 2021, 2023 Arm Limited and/or its affiliates
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
#ifndef APP_CTX_HPP
#define APP_CTX_HPP

#include <map>
#include <memory>
#include <string>
namespace arm {
namespace app {

    class IAttribute
    {
    public:
        virtual ~IAttribute() = default;
    };

    template<typename T>
    class Attribute : public IAttribute
    {
    public:
        ~Attribute() override = default;

        explicit Attribute(const T value): m_value(value){}

        T Get()
        {
            return m_value;
        }
    private:
        T m_value;
    };

    /* Application context class */
    class ApplicationContext {
    public:

        /**
         * @brief     Saves given value as a named attribute in the context.
         * @tparam    T value type.
         * @param[in] name     Context attribute name.
         * @param[in] object   Value to save in the context.
         */
        template<typename T>
        void Set(const std::string &name, T object)
        {
            /* Attribute exists; reset the smart pointer */
            if (this->Has(name)) {
              this->m_attributes.at(name).reset();
            }
            this->m_attributes[name] = std::make_unique<Attribute<T>>(object);
        }

        /**
         * @brief      Gets the saved attribute from the context by the given name.
         * @tparam     T value type.
         * @param[in]  name   Context attribute name.
         * @return     Value saved in the context.
         */

        template <typename T>
        T Get(const std::string& name)
        {
            //TODO Add logic to handle access of non-existent attribute
            auto attributeValue = (Attribute<T>*)m_attributes.at(name).get();
            return attributeValue->Get(); 
        }

        /**
         * @brief      Checks if an attribute for a given name exists in the context.
         * @param[in]  name   Attribute name.
         * @return     true if attribute exists, false otherwise
         */
        bool Has(const std::string& name)
        {
            return m_attributes.find(name) != m_attributes.end();
        }

        ApplicationContext() = default;

        ~ApplicationContext() = default;

    private:
        std::map<std::string, std::unique_ptr<IAttribute>> m_attributes;
    };

} /* namespace app */
} /* namespace arm */

#endif /* APP_CTX_HPP */
