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
#include <fstream>
#include <sys/time.h>
#include <climits>

#include "MeraDrpRuntimeWrapper.h"
#include "PreRuntime.h"
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
std::ostream& operator<<(std::ostream& os, InOutDataType type)
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
    while (getline(infile,line))
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
void softmax(float* val, int32_t size)
{
    float max_num = -INT_MAX;
    float sum = 0;
    int32_t i;
    for ( i = 0 ; i<size ; i++ )
    {
        max_num = std::max(max_num, val[i]);
    }

    for ( i = 0 ; i<size ; i++ )
    {
        val[i]= (float) exp(val[i] - max_num);
        sum+= val[i];
    }

    for ( i = 0 ; i<size ; i++ )
    {
        val[i]= val[i]/sum;
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
int8_t read_bmp(std::string filename, uint32_t width, uint32_t height, uint32_t channel)
{
    int32_t i = 0;
    FILE *fp = NULL;
    size_t ret = 0;
    uint32_t header_size = FILEHEADERSIZE + INFOHEADERSIZE_W_V3;
    /*  Read header for Windows Bitmap v3 file. */
    uint8_t bmp_header[header_size];
    uint8_t * bmp_line_data;

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
        std::cerr << "[ERROR] Failed to run fread(): errno="<<(uint8_t) errno << std::endl;
        fclose(fp);
        return -1;
    }
    /* Single row image data */
    bmp_line_data = (uint8_t *) malloc(sizeof(uint8_t) * line_width);
    if (NULL == bmp_line_data)
    {
        std::cerr << "[ERROR] Failed to malloc for fread(): errno="<<(uint8_t) errno << std::endl;
        fclose(fp);
        return -1;
    }

    for (i = height-1; i >= 0; i--)
    {
        errno = 0;
        ret = fread(bmp_line_data, sizeof(uint8_t), line_width, fp);
        if (!ret)
        {
            std::cerr << "[ERROR] Failed to run fread(): errno="<<(uint8_t) errno << std::endl;
            free(bmp_line_data);
            fclose(fp);
            return -1;
        }
        memcpy(img_buffer[0]+i*width*channel, bmp_line_data, sizeof(uint8_t)*width*channel);
    }

    free(bmp_line_data);
    fclose(fp);
    return 0;
}
#endif /*DBG_CODE_GEN_SNIPPET*/


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
uint32_t get_drpai_start_addr()
{
    int fd  = 0;
    int ret = 0;
    drpai_data_t drpai_data;

    errno = 0;

    fd = open("/dev/drpai0", O_RDWR);
    if (0 > fd )
    {
        LOG(FATAL) << "[ERROR] Failed to open DRP-AI Driver : errno=" << errno;
        return (uint32_t)NULL;
    }

    /* Get DRP-AI Memory Area Address via DRP-AI Driver */
    ret = ioctl(fd , DRPAI_GET_DRPAI_AREA, &drpai_data);
    if (-1 == ret)
    {
        LOG(FATAL) << "[ERROR] Failed to get DRP-AI Memory Area : errno=" << errno ;
        return (uint32_t)NULL;
    }

    return drpai_data.address;
}

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
/*****************************************
* Function Name : get_udmabuf_addr
* Description   : Function to obtain the u-dma-buf start address.
* Arguments     : -
* Return value  : uint32_t = u-dma-buf start address in 32-bit.
******************************************/
uint32_t get_udmabuf_addr()
{
    int     fd = 0;
    char    addr[1024];
    int32_t read_ret = 0;
    uint32_t udmabuf_addr_start = 0;
    errno = 0;

    fd = open("/sys/class/u-dma-buf/udmabuf0/phys_addr", O_RDONLY);
    if (0 > fd)
    {
        std::cerr << "[ERROR] Failed to open udmabuf phys_addr " << std::endl;
        return 0;
    }
    read_ret = read(fd, addr, 1024);
    if (0 > read_ret)
    {
        std::cerr << "[ERROR] Failed to read udmabuf phys_addr " << std::endl;
        close(fd);
        return 0;
    }
    sscanf(addr, "%lx", &udmabuf_addr_start);
    close(fd);
    /* Filter the bit heigher than 32 bit */
    udmabuf_addr_start &=0xFFFFFFFF;
    return udmabuf_addr_start;
}


/*****************************************
* Function Name : ceil3
* Description   : ceil num specify digit.
* Arguments     : num - number
                  base - ceil digit
* Return value  : int32_t = ceil num specify digit.
******************************************/
static int32_t ceil3(int32_t num, int32_t base)
{
    double x = (double)(num) / (double)(base);
    double y = ceil(x) * (double)(base);
    return (int32_t)(y);
}


/*****************************************
* Function Name : init_udmabuf
* Description   : Function to initilize the u-dma-buf area. 
                  This function initialises the number of u-dma-buf area given by `MODEL_IN_NUM`.
* Arguments     : udmabuf_size - Each memory size to get it from u-dma-buf.
                  udmabuf_fd - File descriptor for u-dma-buf file.
                  udmabuf_addr_start - u-dma-buf start address.
                  offset - Offset to align the each u-dma-buf area.
* Return value  : int8_t = if 0, success. otherwise error occured.
******************************************/
int8_t init_udmabuf(const uint32_t udmabuf_size, int& udmabuf_fd, uint64_t& udmabuf_addr_start, int32_t& offset)
{

    /*Obtain u-dma-buf memory area starting address*/
    udmabuf_addr_start = get_udmabuf_addr();
    if (0 == udmabuf_addr_start)
    {
        std::cerr << "[ERROR] Failed to get u-dma-buf." << std::endl;
        return -1;
    }
    /* Allocate image buffer in u-dma-buf memory area */
    udmabuf_fd = open("/dev/udmabuf0", O_RDWR );
    if (0 > udmabuf_fd)
    {
        std::cerr << "[ERROR] Failed to open udmabuf " << std::endl;
        return -1;
    }
    /* page size alignment.*/
    offset = ceil3(udmabuf_size, sysconf(_SC_PAGE_SIZE));

    for (int n = 0; n < MODEL_IN_NUM; n++)
    {
        img_buffer[n] = (unsigned char*) mmap(NULL, udmabuf_size, PROT_READ|PROT_WRITE, MAP_SHARED,  udmabuf_fd, n * offset);
        if (MAP_FAILED == img_buffer[n])
        {
            std::cerr << "[ERROR] Failed to run mmap: udmabuf " << std::endl;
            close(udmabuf_fd);
            return -1;
        }

        /* Write once to allocate physical memory to u-dma-buf virtual space.
        * Note: Do not use memset() for this.
        *       Because it does not work as expected. */
        {
            unsigned char* word_ptr = img_buffer[n];
            for(int i = 0; i < udmabuf_size; i++)
            {
                word_ptr[i] = 0;
            }
        }
    }

    return 0;
}
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

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


int main(int argc, char** argv)
{
    uint8_t ret = 0;
    /* Time Measurement */
    struct timespec start_time, end_time;
    double diff = 0;

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
    /* About u-dma-buf
        Pre-processing Runtime requires the input buffer to be allocated in continuous memory area.
        This application uses imagebuf (u-dma-buf) memory area.
        Refer to RZ/V2x DRP-AI Support Package for imagebuf details. */
    /*File descriptor for u-dma-buf*/
    int udmabuf_fd = 0;
    /* u-dma-buf start addres */
    uint64_t udmabuf_addr_start = 0;
    uint32_t udmabuf_size = *std::max_element(img_sizes.begin(), img_sizes.end());
    int32_t offset = 0;

    ret = init_udmabuf(udmabuf_size, udmabuf_fd, udmabuf_addr_start, offset);
    if (ret != 0) 
    {
        std::cerr << "[ERROR] Failed to initilize udma buf." << std::endl;
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
            munmap(img_buffer[i], udmabuf_size);
        }
        close(udmabuf_fd);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
        return -1;
    }

#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET)
    /*Load pre_dir object to DRP-AI */
    for(int n = 0; n < MODEL_IN_NUM; n++)
    {
        ret = preruntime[n].Load(pre_dir[n], drpaimem_addr_start + pre_start_addr[n]);
        if (0 < ret)
        {
            std::cerr << "[ERROR] Failed to run Pre-processing Runtime Load()." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
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
        ret = preruntime[n].Load(pre_dir[n], drpaimem_addr_start + pre_start_addr[n]);
        if (0 < ret)
        {
            std::cerr << "[ERROR] Failed to run Pre-processing Runtime Load()." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
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
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
            return -1;
        }
        else
        {
            std::cerr << "[ERROR] Input data type : neither FP32 nor FP16." << std::endl;
#if defined(CODE_GEN_MODE_USE_PREPRO_RUNTIME) || defined(DBG_CODE_GEN_SNIPPET) || defined(CODE_GEN_MODE_MIXED_USE_PREPRO_RUNTIME)
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/
            return -1;
        }

#ifdef DBG_CODE_GEN_SNIPPET
        /* Read image data from file */
        ret = read_bmp(filename, INPUT_IMAGE0_W, INPUT_IMAGE0_H, INPUT_IMAGE0_C);
        if (ret > 0)
        {
            std::cerr << "[ERROR] Failed to read image :"<<filename << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
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
        std::cout << "\033[33mNote: This example execute with dummy data from udmabuf memory area. Therefore, specify the actual image to be loaded. \033[0m" << std::endl;
        /* Pre-processing */
        /*Define parameter to be changed in Pre-processing Runtime*/
        in_param[n].pre_in_addr    = udmabuf_addr_start + offset * n;

        timespec_get(&start_time, TIME_UTC);
        /*Run pre-processing*/
        ret = preruntime[n].Pre(&in_param[n], &output_ptr[n], &out_size[n]);
        if (0 < ret)
        {
            std::cerr << "[ERROR] Failed to run Pre-processing Runtime Pre()." << std::endl;
            for(int i = 0; i < MODEL_IN_NUM; i++)
            {
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
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
            std::cout << "\033[33mNote: This example execute with dummy data from udmabuf memory area. Therefore, specify the actual image to be loaded.\033[0m" << std::endl;
            /* Pre-processing */
            /*Define parameter to be changed in Pre-processing Runtime*/
            in_param[n].pre_in_addr    = udmabuf_addr_start + offset * n;

            timespec_get(&start_time, TIME_UTC);
            /*Run pre-processing*/
            ret = preruntime[n].Pre(&in_param[n], &output_ptr[n], &out_size[n]);
            if (0 < ret)
            {
                std::cerr << "[ERROR] Failed to run Pre-processing Runtime Pre()." << std::endl;
                for(int i = 0; i < MODEL_IN_NUM; i++)
                {
                    munmap(img_buffer[i], udmabuf_size);
                }
                close(udmabuf_fd);
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
                munmap(img_buffer[i], udmabuf_size);
            }
            close(udmabuf_fd);
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
            munmap(img_buffer[i], udmabuf_size);
        }
        close(udmabuf_fd);
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
    std::cout << "Result ----------------------- "<< std::endl;
    for (auto it = result.rbegin(); it != result.rend(); it++)
    {
        result_cnt++;
        if (result_cnt > 5) break;
        std::cout << "  Top "<< result_cnt << " ["
            << std::right << std::setw(5) << std::fixed << std::setprecision(1) << (float)(*it).first*100
            <<"%] : [" << label_file_map[(*it).second] << "]" <<std::endl;
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
        munmap(img_buffer[i], udmabuf_size);
    }
    close(udmabuf_fd);
#endif /*CODE_GEN_MODE_USE_PREPRO_RUNTIME*/

    printf("################################################\n");
    printf("#    Finish Sample Application                 #\n");
    printf("################################################\n");

    return 0;
}
