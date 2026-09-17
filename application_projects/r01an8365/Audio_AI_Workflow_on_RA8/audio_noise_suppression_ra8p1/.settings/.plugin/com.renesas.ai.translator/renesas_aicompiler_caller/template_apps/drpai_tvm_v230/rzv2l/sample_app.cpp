/*
 * Original Code (C) Copyright Edgecortix, Inc. 2022
 * Modified Code (C) Copyright Renesas Electronics Corporation 2023-2024
 *
 *  *1 DRP-AI TVM is powered by EdgeCortix MERA(TM) Compiler Framework.
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

/***********************************************************************************************************************
* File Name    : sample_app.cpp
//<\WRITE_VERSION/>
* Description  : Auto generated file. DRP-AI TVM[*1] Application Example
***********************************************************************************************************************/

/*****************************************
* includes
******************************************/
#include <linux/drpai.h>
#include <builtin_fp16.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <climits>

#include "MeraDrpRuntimeWrapper.h"
#include "PreRuntime.h"
/* This block of code is only accessible from C code. */
#ifdef __cplusplus
extern "C" {
#endif
#include "mmngr_user_public.h"
#include "mmngr_buf_user_public.h"
#ifdef __cplusplus
}
#endif
/*****************************************
* Macro
* INPUT_IMAGEX_H: input image height for preprocessing
* INPUT_IMAGEX_W: input image width for preprocessing
* INPUT_IMAGEX_C: number of channels of input image for preprocessing
* MODEL_IN_NUM: number of inputs for AI model
* MODEL_INX_H: input image height for AI model
* MODEL_INX_W: input image width for AI model
* MODEL_INX_C: number of channels of input image for AI model
* TVM_MODEL_OBJ_OFFSET: offset address that DRP-AI TVM[*1] Model Object is deployed.
******************************************/
#define DBG_CODE_GEN_SNIPPET
#ifdef DBG_CODE_GEN_SNIPPET
/*Input image info*/
#define INPUT_IMAGE0_H       (480)
#define INPUT_IMAGE0_W       (640)
#define INPUT_IMAGE0_C       (3)
/*Model input info*/
#define MODEL_IN_NUM         (1)
#define MODEL_IN0_H          (224)
#define MODEL_IN0_W          (224)
#define MODEL_IN0_C          (3)

#define TVM_MODEL_OBJ_OFFSET (0x38E0000)

/*BMP Header size for Windows Bitmap v3*/
#define FILEHEADERSIZE          (14)
#define INFOHEADERSIZE_W_V3     (40)

#else
//<\WRITE_MACRO/>
#endif /*DBG_CODE_GEN_SNIPPET*/

/* DRP-AI TVM[*1] Runtime object */
MeraDrpRuntimeWrapper runtime;
/* dma_buffer : dma buffer itself and its feature */
typedef struct 
{
        /* The index of the buffer. */
        uint32_t idx;
        /* The file descriptor for the DMA buffer. */
        uint32_t dbuf_fd;
        /* The size of the buffer in bytes. */
        uint32_t size;
        /* The physical address of DMA buffer. */
        uint32_t phy_addr;
        /* The pointer to the memory for the buffer. */
        void *mem;           
} dma_buffer;

#if defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
/*****************************************
* When referring to this example, note the following.
* 
* This examle allocate unused arrays such like `preruntime[]` or `in_param[]` etc.
* to prioritize readability.
*
* Please delete unused arrays after thorough testing.
******************************************/
#endif /*CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME*/
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
/* Pre-processing Runtime object */
std::vector<PreRuntime> preruntime(MODEL_IN_NUM);
/* MMNGR buffer for DRP-AI Pre-processing */
static dma_buffer *drpai_buf;
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

#ifdef DBG_CODE_GEN_SNIPPET
/* Model Binary */
const static std::string model_dir = "resnet18_onnx";
/* Pre-processing Runtime Object */
const static std::vector<std::string> pre_dir = {model_dir + "/preprocess"};
const static std::vector<uint32_t> pre_start_addr = {0x00000000};
/* Image buffer (u-dma-buf) */
static unsigned char * img_buffer[MODEL_IN_NUM];
const static std::vector<uint32_t> img_sizes = {
    INPUT_IMAGE0_H * INPUT_IMAGE0_W * INPUT_IMAGE0_C
};
static std::vector<s_preproc_param_t> in_param = {
    {
        .pre_in_shape_w = INPUT_IMAGE0_W,
        .pre_in_shape_h = INPUT_IMAGE0_H,
        .pre_in_format  = FORMAT_BGR,
        .pre_out_format = FORMAT_RGB,
    }
};

/* The following definitions are used for debbuging */
/* Label list file for ImageNet*/
const static std::string labels = "synset_words_imagenet.txt";
/* Input image file */
const static std::string filename = "sample.bmp";
/* Map to store label list */
static std::map<int, std::string> label_file_map;
/* Map list to store the classification result. */
static std::map<float,int> result;

#else
//<\WRITE_GLOBAL_VAR/>
#endif /*DBG_CODE_GEN_SNIPPET*/


/* Edgecortex Functions */
std::ostream &operator<<(std::ostream &os, InOutDataType type)
{
    switch (type)
    {
        case InOutDataType::FLOAT32:
            os << "FLOAT32";
            break;
        case InOutDataType::FLOAT16:
            os << "FLOAT16";
            break;
        case InOutDataType::OTHER:
            os << "OTHER";
            break;
        default:
            ;
    }
    return os;
}

#ifdef DBG_CODE_GEN_SNIPPET
/*****************************************
* Function Name     : load_label_file
* Description       : Load label list text file and return the label list that contains the label.
* Arguments         : label_file_name = filename of label list. must be in txt format
* Return value      : std::map<int, std::string> list = list text file which contains labels
*                     empty if error occured
******************************************/
std::map<int, std::string> load_label_file(std::string label_file_name)
{
    int n = 0;
    std::map<int, std::string> list;
    std::ifstream infile(label_file_name);

    if (!infile.is_open())
    {
        return list;
    }

    std::string line;
    while (getline(infile, line))
    {
        list[n++] = line;
        if (infile.fail())
        {
            std::map<int, std::string> empty;
            return empty;
        }
    }

    return list;
}

/*****************************************
* Function Name : softmax
* Description   : Function for Post Processing
* Arguments     : val[] = array to be computed Softmax
*                 size = size of array
* Return value  : -
******************************************/
void softmax(float *val, int32_t size)
{
    float max_num = -INT_MAX;
    float sum = 0;
    int32_t i;
    for (i = 0; i < size; i++)
    {
        max_num = std::max(max_num, val[i]);
    }

    for (i = 0; i < size; i++)
    {
        val[i] = (float)exp(val[i] - max_num);
        sum += val[i];
    }

    for (i = 0; i < size; i++)
    {
        val[i] = val[i] / sum;
    }
    return;
}

/*****************************************
* Function Name : read_bmp
* Description   : Function to load BMP file into img_buffer
* NOTE          : This is just the simplest example to read Windows Bitmap v3 file.
*                 This function does not have header check.
* Arguments     : filename = name of BMP file to be read
*                 width  = BMP image width
*                 height = BMP image height
*                 channel = BMP image color channel
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t read_bmp(std::string filename, uint32_t width, uint32_t height, uint32_t channel, uint8_t *buffer)
{
    FILE *fp = NULL;
    size_t ret = 0;
    uint32_t header_size = FILEHEADERSIZE + INFOHEADERSIZE_W_V3;
    /*  Read header for Windows Bitmap v3 file. */
    uint8_t bmp_header[header_size];

    /* Number of byte in single row */
    /* NOTE: Number of byte in single row of Windows Bitmap image must be aligned to 4 bytes. */
    int32_t line_width = width * channel + width % 4;

    fp = fopen(filename.c_str(), "rb");
    if (NULL == fp)
    {
        return -1;
    }
    /* Read all header */
    errno = 0;
    ret = fread(bmp_header, sizeof(uint8_t), header_size, fp);
    if (!ret)
    {
        std::cerr << "[ERROR] Failed to run fread(): errno=" << (uint8_t)errno << std::endl;
        fclose(fp);
        return -1;
    }
    /* Single row image data */
    uint8_t bmp_line_data[sizeof(uint8_t) * line_width];

    for (int i = height - 1; i >= 0; i--)
    {
        errno = 0;
        ret = fread(bmp_line_data, sizeof(uint8_t), line_width, fp);
        if (!ret)
        {
            std::cerr << "[ERROR] Failed to run fread(): errno=" << errno << std::endl;
            fclose(fp);
            return -1;
        }
        memcpy(buffer + i * width * channel, bmp_line_data, sizeof(uint8_t) * width * channel);
    }

    fclose(fp);
    return 0;
}
#endif /*DBG_CODE_GEN_SNIPPET*/

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
/*****************************************
* Function Name : buffer_alloc_dmabuf
* Description   : Allocate a DMA buffer in continuous memory area.
* Arguments     : buffer = pointer to the dma_buffer struct
*                 buf_size = size of the allocation
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t buffer_alloc_dmabuf( dma_buffer *buffer, int buf_size)
{
    MMNGR_ID id;
    uint32_t phard_addr;
    void *puser_virt_addr;
    int m_dma_fd;

    buffer->size = buf_size;
    mmngr_alloc_in_user_ext(&id, buffer->size, &phard_addr, &puser_virt_addr, MMNGR_VA_SUPPORT_CACHED, NULL);
    memset((void*)puser_virt_addr, 0, buffer->size);
    buffer->idx = id;
    buffer->mem = (void *)puser_virt_addr;
    buffer->phy_addr = phard_addr;
    if (!buffer->mem)
    {
        return -1;
    }

    mmngr_export_start_in_user_ext(&id, buffer->size, phard_addr, &m_dma_fd, NULL);
    buffer->dbuf_fd = m_dma_fd;
    return 0;
}

/*****************************************
* Function Name : buffer_free_dmabuf
* Description   : free a DMA buffer in continuous memory area.
*                 MUST be called if buffer_alloc_dmabuf is called.
* Arguments     : buffer = pointer to the dma_buffer struct
* Return value  : -
******************************************/
void buffer_free_dmabuf(dma_buffer *buffer)
{
    mmngr_free_in_user_ext(buffer->idx);
    return;
}

/*****************************************
* Function Name : buffer_flush_dmabuf
* Description   : flush a DMA buffer in continuous memory area
*                 MUST be called when writing data to DMA buffer
* Arguments     : idx = id of the buffer to be flushed.
*                 size = size to be flushed.
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int buffer_flush_dmabuf(uint32_t idx, uint32_t size)
{
    int mm_ret = 0;
    
    /* Flush capture image area cache */
    mm_ret = mmngr_flush(idx, 0, size);
    return mm_ret;
}
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

/*****************************************
* Function Name     : float16_to_float32
* Description       : Function by Edgecortex. Cast uint16_t a into float value.
* Arguments         : a = uint16_t number
* Return value      : float = float32 number
******************************************/
float float16_to_float32(uint16_t a)
{
    return __extendXfYf2__<uint16_t, uint16_t, 10, float, uint32_t, 23>(a);
}


/*****************************************
* Function Name : get_drpai_start_addr
* Description   : Function to get the start address of DRPAImem.
* Arguments     : -
* Return value  : uint32_t = DRPAImem start address in 32-bit.
******************************************/
uint64_t get_drpai_start_addr()
{
    int fd  = 0;
    int ret = 0;
    drpai_data_t drpai_data;

    errno = 0;

    fd = open("/dev/drpai0", O_RDWR);
    if (0 > fd)
    {
        LOG(FATAL) << "[ERROR] Failed to open DRP-AI Driver : errno=" << errno;
        return (uint64_t)NULL;
    }

    /* Get DRP-AI Memory Area Address via DRP-AI Driver */
    ret = ioctl(fd, DRPAI_GET_DRPAI_AREA, &drpai_data);
    if (-1 == ret)
    {
        LOG(FATAL) << "[ERROR] Failed to get DRP-AI Memory Area : errno=" << errno ;
        return (uint64_t)NULL;
    }
    close(fd);

    return drpai_data.address;
}

/*****************************************
* Function Name : timedifference_msec
* Description   : Function to compute the processing time in mili-seconds
* Arguments     : t0 = processing start time
*                 t1 = processing end time
* Return value  : processing time in mili-seconds
******************************************/
static double timedifference_msec(struct timespec t0, struct timespec t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) / 1000.0 / 1000.0;
}


int main(int argc, char **argv)
{
    uint8_t ret = 0;
    /* Time Measurement */
    struct timespec start_time, end_time;
    double diff = 0;
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
    uint32_t max_img_size = *std::max_element(img_sizes.begin(), img_sizes.end());
    for (int n = 0; n < MODEL_IN_NUM; n++){
        img_buffer[n] = new unsigned char[max_img_size];
    }

    /*Initialize buffer for DRP-AI Pre-processing Runtime. */
    drpai_buf = (dma_buffer*)malloc(sizeof(dma_buffer));
    ret = buffer_alloc_dmabuf(drpai_buf, max_img_size);
    if (-1 == ret)
    {
        std::cerr << "[ERROR] Failed to Allocate DMA buffer for the drpai_buf" << std::endl;
        for(int i = 0; i < MODEL_IN_NUM; i++)
        {
            delete[] img_buffer[i];
        }
        free(drpai_buf);
        return -1;
    }
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

    printf("################################################\n");
    printf("#    Sample Application                        #\n");
    printf("#          for checking inference speed        #\n");
    printf("################################################\n");

    /*Get DRP-AI memory start address*/
    uint64_t drpaimem_addr_start = 0;
    drpaimem_addr_start = get_drpai_start_addr();
    if (drpaimem_addr_start == (uint64_t)NULL) 
    {
        std::cerr << "[ERROR] Failed to get the DRP-AI memory start address." << std::endl;
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
        for(int i = 0; i < MODEL_IN_NUM; i++)
        {
            delete[] img_buffer[i];
        }
        buffer_free_dmabuf(drpai_buf);
        free(drpai_buf);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
        return -1;
    }

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET)
    /*Load pre_dir object to DRP-AI */
    for(int n = 0; n < MODEL_IN_NUM; n++)
    {
        ret = preruntime[n].Load(pre_dir[n], drpaimem_addr_start + pre_start_addr[n], MODE_PRE);
        if (0 < ret)
        {
            std::cerr << "[ERROR] Failed to run Pre-processing Runtime Load()." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
            return -1;
        }
    }
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

#if defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
    /*Load pre_dir object to DRP-AI */
    for(int n = 0; n < MODEL_IN_NUM; n++)
    {
        if(pre_dir[n].empty()){
            continue;
        }
        ret = preruntime[n].Load(pre_dir[n], drpaimem_addr_start + pre_start_addr[n], MODE_PRE);
        if (0 < ret)
        {
            std::cerr << "[ERROR] Failed to run Pre-processing Runtime Load()." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
            return -1;
        }
    }
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

    /*Load model_dir structure and its weight to runtime object */
    runtime.LoadModel(model_dir, drpaimem_addr_start + TVM_MODEL_OBJ_OFFSET);

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
    /*Output variables for Pre-processing Runtime */
    void* output_ptr[MODEL_IN_NUM];
    uint32_t out_size[MODEL_IN_NUM];
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
    for(int n = 0; n < MODEL_IN_NUM; n++)
    {
        /*Get input data type */
        auto input_data_type = runtime.GetInputDataType(n);

        /*Input data type can be either FLOAT32 or FLOAT16, which depends on the model */
        if (InOutDataType::FLOAT32 == input_data_type)
        {
            /* Do nothing */
        }
        else if (InOutDataType::FLOAT16 == input_data_type)
        {
            std::cerr << "[ERROR] Input data type : FP16." << std::endl;
            /*If your model input data type is FP16, use std::vector<uint16_t> for reading input data. */
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
            return -1;
        }
        else
        {
            std::cerr << "[ERROR] Input data type : neither FP32 nor FP16." << std::endl;
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
            return -1;
        }

#ifdef DBG_CODE_GEN_SNIPPET
        /* Read image data from file */
        ret = read_bmp(filename, INPUT_IMAGE0_W, INPUT_IMAGE0_H, INPUT_IMAGE0_C, img_buffer[0]);
        if (ret > 0)
        {
            std::cerr << "[ERROR] Failed to read image :" << filename << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
            return -1;
        }
#endif /*DBG_CODE_GEN_SNIPPET*/
        /*****************************************
        * Please write loading data function from here.
        * e.g. read image data from file.
        * This example execute with dummy data from memory area.
        * Therefore, specify the actual image to be loaded.
        *
        * do_loading_data_func()
        ******************************************/
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET)
        std::cout << "\033[33mNote: This example execute with dummy data from memory area. Therefore, specify the actual image to be loaded. \033[0m" << std::endl;
        /* Pre-processing */
        /* Copy input data to drpai_buf for DRP-AI Pre-processing Runtime.*/
        memcpy( drpai_buf->mem, img_buffer[n], drpai_buf->size);
        /* Flush buffer */
        ret = buffer_flush_dmabuf(drpai_buf->idx, drpai_buf->size);
        if (0 != ret)
        {
            std::cerr << "[ERROR] Failed to flush dmabuf." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
            return -1;
        }
        /*Define parameter to be changed in Pre-processing Runtime*/
        in_param[n].pre_in_addr    = (uintptr_t) drpai_buf->phy_addr;

        timespec_get(&start_time, TIME_UTC);
        /*Run pre-processing*/
        ret = preruntime[n].Pre(&in_param[n], &output_ptr[n], &out_size[n]);
        if (0 < ret)
        {
            std::cerr << "[ERROR] Failed to run Pre-processing Runtime Pre()." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
            return -1;
        }
        timespec_get(&end_time, TIME_UTC);
        /* Print Inference processing time */
        diff = timedifference_msec(start_time, end_time);
        std::cout<<"\033[32m[TIME] Pre Processing Time: "<< std::fixed << std::setprecision(2)<< diff << " msec.\033[0m" << std::endl;

        /*Set Pre-processing output to be inference input. */
        runtime.SetInput(n, (float*)output_ptr[n]);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
#if defined(CODE_GEN_MODE_DONT_USE_PREPRO_RUNTIME)
        std::cout << "\033[33mNote: This example execute with dummy data. Therefore, specify the actual image to be loaded. \033[0m" << std::endl;
        std::shared_ptr<float> input_data_buf(new float[img_sizes[n]], std::default_delete<float[]>());
        /*Set Pre-processing output to be inference input. */
        runtime.SetInput(n, (float*)input_data_buf.get());
#endif /*CODE_GEN_MODE_DONT_USE_PREPRO_RUNTIME*/

#if defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
        if(pre_dir[n].empty())
        {
            std::cout << "\033[33mNote: This example execute with dummy data. Therefore, specify the actual image to be loaded.\033[0m" << std::endl;
            std::shared_ptr<float> input_data_buf(new float[img_sizes[n]], std::default_delete<float[]>());
            /*Set Pre-processing output to be inference input. */
            runtime.SetInput(n, (float*)input_data_buf.get());
        }
        else
        {
            std::cout << "\033[33mNote: This example execute with dummy data from memory area. Therefore, specify the actual image to be loaded.\033[0m" << std::endl;
            /* Pre-processing */
            /*Copy input data to drpai_buf for DRP-AI Pre-processing Runtime.*/
            memcpy( drpai_buf->mem, img_buffer[n], drpai_buf->size);
            /* Flush buffer */
            ret = buffer_flush_dmabuf(drpai_buf->idx, drpai_buf->size);
            if (0 != ret)
            {
                std::cerr << "[ERROR] Failed to flush dmabuf." << std::endl;
                for(int i = 0; i < MODEL_IN_NUM; i++)
                {
                    delete[] img_buffer[i];
                }
                buffer_free_dmabuf(drpai_buf);
                free(drpai_buf);
                return -1;
            }
            /*Define parameter to be changed in Pre-processing Runtime*/
            in_param[n].pre_in_addr    = (uintptr_t) drpai_buf->phy_addr;

            timespec_get(&start_time, TIME_UTC);
            /*Run pre-processing*/
            ret = preruntime[n].Pre(&in_param[n], &output_ptr[n], &out_size[n]);
            if (0 < ret)
            {
                std::cerr << "[ERROR] Failed to run Pre-processing Runtime Pre()." << std::endl;
                for(int i = 0; i < MODEL_IN_NUM; i++)
                {
                    delete[] img_buffer[i];
                }
                buffer_free_dmabuf(drpai_buf);
                free(drpai_buf);
                return -1;
            }
            timespec_get(&end_time, TIME_UTC);
            /* Print Inference processing time */
            diff = timedifference_msec(start_time, end_time);
            std::cout<<"\033[32m[TIME] Pre Processing Time: "<< std::fixed << std::setprecision(2)<< diff << " msec.\033[0m" << std::endl;

            /*Set Pre-processing output to be inference input. */
            runtime.SetInput(n, (float*)output_ptr[n]);
        }
#endif /*CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME*/
    }

    timespec_get(&start_time, TIME_UTC);
    std::cout << "Running tvm runtime" << std::endl;
    runtime.Run();
    timespec_get(&end_time, TIME_UTC);

    /* Print Inference processing time */
    diff = timedifference_msec(start_time, end_time);
    std::cout<<"\033[32m[TIME] AI Processing Time: "<< std::fixed << std::setprecision(2)<< diff << " msec.\033[0m" << std::endl;

    /* Process to read the DRP-AI output data. */
    /* Get the number of output of the target model. */
    auto output_num = runtime.GetNumOutput();
    /* Buffer to store the FP32 output data from inference. */
    std::shared_ptr<float> drpai_output_buf;
    /* Counter for inference output buffer */
    int64_t size_count = 0;
    /* Count output size */
    for (int i = 0; i < output_num; i++){
        /* output_buffer below is tuple, which is { data type, address of output data, number of elements } */
        auto output_buffer = runtime.GetOutput(i);
        int64_t output_size = std::get<2>(output_buffer);
        size_count += output_size;
    }
    drpai_output_buf.reset(new float[size_count], std::default_delete<float[]>());
    /* GetOutput loop */
    size_count = 0;
    for (int i = 0; i < output_num; i++)
    {
        /* output_buffer below is tuple, which is { data type, address of output data, number of elements } */
        auto output_buffer = runtime.GetOutput(i);
        int64_t output_size = std::get<2>(output_buffer);

        if (InOutDataType::FLOAT16 == std::get<0>(output_buffer))
        {
            std::cout << "Output data type : FP16." << std::endl;
            /* Extract data in FP16 <uint16_t>. */
            uint16_t *data_ptr = reinterpret_cast<uint16_t *>(std::get<1>(output_buffer));

            /* Post-processing for FP16 */
            /* Cast FP16 output data to FP32. */
            for (int j = 0; j < output_size; j++)
            {
                drpai_output_buf.get()[j + size_count] = float16_to_float32(data_ptr[j]);
            }
        }
        else if (InOutDataType::FLOAT32 == std::get<0>(output_buffer))
        {
            std::cout << "Output data type : FP32." << std::endl;
            /* Extract data in FP32 <float>. */
            float *data_ptr = reinterpret_cast<float *>(std::get<1>(output_buffer));
            /*Copy output data to buffer for post-processing. */
            for (int j = 0; j < output_size; j++)
            {
                drpai_output_buf.get()[j + size_count] = data_ptr[j];
            }
        }
        else
        {
            std::cerr << "[ERROR] Output data type : not floating point type." << std::endl;
            /*End application*/
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                delete[] img_buffer[i];
            }
            buffer_free_dmabuf(drpai_buf);
            free(drpai_buf);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
            return -1;
        }
        size_count += output_size;
    }

#ifdef DBG_CODE_GEN_SNIPPET
    /* Load Label list */
    label_file_map = load_label_file(labels);
    if (label_file_map.empty())
    {
        std::cerr << "[ERROR] Label file : failed to load "<< labels << std::endl;
        for(int i = 0; i < MODEL_IN_NUM; i++)
        {
            delete[] img_buffer[i];
        }
        buffer_free_dmabuf(drpai_buf);
        free(drpai_buf);
        return -1;
    }

    /* Size to store the output buffer */
    int64_t drpai_output_buf_size = size_count;
    /* Pointer to store the FP32 output data from inference. */
    float * floatarr = drpai_output_buf.get();

    /*Post-processing: common for FP16/FP32*/
    /* Softmax 1000 class scores. */
    softmax(&floatarr[0], drpai_output_buf_size);
    /* Sort in decending order. */
    for (int n = 0; n < drpai_output_buf_size; n++)
    {
        result[floatarr[n]] = n;
    }

    int result_cnt = 0;
    /* Print Top-5 results. */
    std::cout << "Result ----------------------- " << std::endl;
    for (auto it = result.rbegin(); it != result.rend(); it++)
    {
        result_cnt++;
        if (result_cnt > 5)
            break;
        std::cout << "  Top " << result_cnt << " ["
                  << std::right << std::setw(5) << std::fixed << std::setprecision(1) << (float)(*it).first * 100
                  << "%] : [" << label_file_map[(*it).second] << "]" << std::endl;
    }
#endif /*DBG_CODE_GEN_SNIPPET*/
    /*****************************************
    * Please write post-processing function from here.
    * e.g. The softmax function is generally used in ResNet18.
    * This example does not support post-processing..
    *
    * do_postprocessing()
    ******************************************/
    std::cout << "\033[33mNote: This example does not support post-processing.\033[0m" << std::endl;

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET)  || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
    for(int i = 0; i < MODEL_IN_NUM; i++)
    {
        delete[] img_buffer[i];
    }
    buffer_free_dmabuf(drpai_buf);
    free(drpai_buf);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

    printf("################################################\n");
    printf("#    Finish Sample Application                 #\n");
    printf("################################################\n");

    return 0;
}
