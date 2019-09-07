//
// Created by ly on 19-5-14.
//
#include <com_example_myapplication1_FristJni.h>

std::mutex mtx;
std::mutex end_mtx;
int batchSize;

vector<std::unique_ptr<zdl::SNPE::SNPE>>  total_Snpe(12);

struct ItemRepository{
    int flag;
    std::vector<std::string> pic_str;

};

struct ResItemRepository{
    int flag;
    int location;
    float score;

    ResItemRepository(){
        flag=0;
        location=0;
        score=0.0;
    }

    bool operator < (const ResItemRepository &item) const{
        return flag<item.flag;
    }

};

list<ItemRepository> list_pic;
list<ResItemRepository> list_res;


inline int time_cost(char* str,int flag)
{
    static struct  timeval beginTime = {0,0};
    static struct  timeval endTime = {0,0};

    if(flag==0)
    {
        gettimeofday(&beginTime, NULL);
    }
    else
    {
        gettimeofday(&endTime, NULL);
        long long tm_begin = beginTime.tv_sec*1000+beginTime.tv_usec/1000;
        long long tm_end = endTime.tv_sec*1000+endTime.tv_usec/1000;
        LOGI("%s cost:%lldms",str,tm_end-tm_begin);

        printf("%s cost: %lldms\n",str,tm_end-tm_begin);
    }

    return 0;
}


std::unique_ptr<zdl::SNPE::SNPE> BuildCPU(
               std::unique_ptr<zdl::DlContainer::IDlContainer> &container,
               zdl::DlSystem::UDLBundle udlBundle,
               bool useUserSuppliedBuffers,
               zdl::DlSystem::PlatformConfig platformConfig,
               bool usingInitCaching
)
{
    static zdl::DlSystem::Runtime_t runtime = zdl::DlSystem::Runtime_t::CPU;
    std::unique_ptr<zdl::SNPE::SNPE> snpe_c;
    runtime = checkRuntime(runtime);
    snpe_c=setBuilderOptions(container,runtime,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
    return snpe_c;
}

std::unique_ptr<zdl::SNPE::SNPE> BuildGPU(
               std::unique_ptr<zdl::DlContainer::IDlContainer> &container,
               zdl::DlSystem::UDLBundle udlBundle,
               bool useUserSuppliedBuffers,
               zdl::DlSystem::PlatformConfig platformConfig,
               bool usingInitCaching
){
    static zdl::DlSystem::Runtime_t runtime = zdl::DlSystem::Runtime_t::GPU;
    std::unique_ptr<zdl::SNPE::SNPE> snpe_g;
    runtime = checkRuntime(runtime);
    snpe_g=setBuilderOptions(container,runtime,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
    return snpe_g;
}


std::unique_ptr<zdl::SNPE::SNPE> BuildDSP(
               std::unique_ptr<zdl::DlContainer::IDlContainer> &container,
               zdl::DlSystem::UDLBundle udlBundle,
               bool useUserSuppliedBuffers,
               zdl::DlSystem::PlatformConfig platformConfig,
               bool usingInitCaching
){
    static zdl::DlSystem::Runtime_t runtime = zdl::DlSystem::Runtime_t::DSP;
    std::unique_ptr<zdl::SNPE::SNPE> snpe_d;
    runtime = checkRuntime(runtime);
    snpe_d=setBuilderOptions(container,runtime,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
    return snpe_d;
}


//mutex m_tmp;
bool get_pic(ItemRepository &singleItemRepository
                                                                 ){
                    std::unique_lock<std::mutex> lock(mtx);
                    //m_tmp.lock();
                    bool flag=!list_pic.empty();
                    if(flag){
                        singleItemRepository=list_pic.front();
                        list_pic.pop_front();
                    }
                    //m_tmp.unlock();
                    lock.unlock();
                    return flag;
}

void output_res(int index1)
{
        struct  timeval beginTime = {0,0};
        struct  timeval endTime = {0,0};

        zdl::DlSystem::TensorShape tensorShape;

        //dimens
        //tensorShape=snpe->getInputDimensions();
        tensorShape=total_Snpe[index1]->getInputDimensions();

        /*
        if(total_Snpe[index1]){
            LOGI("snpe %d no problem",index1);
        }
        */

        //batchisize
        batchSize = tensorShape.getDimensions()[0];

        //LOGI("batchsize is %d",batchSize);

        if(1)
            {
                zdl::DlSystem::TensorMap outputTensorMap_in;
                //LOGI("index %d",index1);

                ItemRepository singleItemRepository;
               // if (get_pic(resgItemRepository1))
                //resgItemRepository1=list_pic.front();
                //{LOGI("pic path %s",(resgItemRepository1.pic_str)[0].c_str());}
                //LOGI("结果有问题...0");

                while(get_pic(singleItemRepository)){


         //std::unique_ptr<zdl::DlSystem::ITensor> inputTensor=loadInputTensor(snpe,resgItemRepository1.pic_str);
#if 1
                //std::unique_ptr<zdl::DlSystem::ITensor> inputTensor=loadInputTensor(total_Snpe[index1],singleItemRepository.pic_str);
                 std::vector<std::string> vec_pic = singleItemRepository.pic_str;
                 std::unique_ptr<zdl::DlSystem::ITensor> inputTensor;
                 const auto &strList_opt = total_Snpe[index1]->getInputTensorNames();
                 const auto &strList=*strList_opt;

                 std::vector<float> inputVec;
                 for(size_t i=0;i<vec_pic.size();i++){
                    std::string filePath(vec_pic[i]);
                    std::vector<int> loadedFile;
                    std::ifstream in(filePath,std::ifstream::binary);
                    in.seekg(0,in.end);
                    size_t length=in.tellg();
                    in.seekg(0,in.beg);
                    if (loadedFile.size()==0)
                    {
                        loadedFile.resize(length/sizeof(float));
                    }
                    else if(loadedFile.size()<length/sizeof(float))
                    {
                        loadedFile.resize(length/sizeof(float));
                    }

                    if(!in.read(reinterpret_cast<char*>(&loadedFile[0]),length))
                    {
                        LOGI("failed to read");
                    }

                    inputVec.insert(inputVec.end(),loadedFile.begin(),loadedFile.end());

                 }

                 const auto &inputDims_opt = total_Snpe[index1]->getInputDimensions(strList.at(0));
                 const auto &inputShape = *inputDims_opt;

                 inputTensor = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);

                 std::copy(inputVec.begin(),inputVec.end(),inputTensor->begin());


                 int num=singleItemRepository.flag;
                    //string a = ((list_pic.front()).pic_str)[0].c_str();

                    // Execute the input tensor on the model with SNPE

                    //gettimeofday(&beginTime, NULL);
                    //bool execStatus = snpe->execute(inputTensor.get(), outputTensorMap_in);
                    bool execStatus = total_Snpe[index1]->execute(inputTensor.get(), outputTensorMap_in);
                    //gettimeofday(&endTime, NULL);

                    //long long tm_begin = beginTime.tv_sec*1000+beginTime.tv_usec/1000;
                    //long long tm_end = endTime.tv_sec*1000+endTime.tv_usec/1000;
                     //LOGI(" 第%d张,图片%s,cost:%lldms",num,(singleItemRepository.pic_str)[0].c_str(),tm_end-tm_begin);



#if 1

                    zdl::DlSystem::StringList tensorNames = outputTensorMap_in.getTensorNames();

                    //print pic and score
                    std::for_each(tensorNames.begin(),tensorNames.end(),[&](const char* name)
                    {
                       for(size_t i=0;i<1;i++){

                            auto tensorPtr=outputTensorMap_in.getTensor(name);
                            size_t batchChunk=tensorPtr->getSize()/batchSize;


                             int j=1;
                             float MAX_NUM=0.0;
                             int MAX_LOCATE=0;
                             for(auto it=tensorPtr->cbegin()+i*batchChunk;it!=tensorPtr->cbegin()+(i+1)*batchChunk;++it)
                             {
                                    float f=*it;

                                    if(f>=MAX_NUM){
                                        MAX_NUM=f;
                                        MAX_LOCATE=j;
                                    }
                                    j++;

                             }
                             //LOGI("第%d张 ,最大位置:%d,得分:%f",pic_num+1,MAX_LOCATE,MAX_NUM);
                             //LOGI("第%d张 ,最大位置:%d,得分:%f",num,MAX_LOCATE,MAX_NUM);
                             ResItemRepository resgItemRepository;

                             resgItemRepository.flag=num;
                             resgItemRepository.location=MAX_LOCATE;
                             resgItemRepository.score=MAX_NUM;

                             std::unique_lock<std::mutex> lock1(end_mtx);
                             list_res.push_back(resgItemRepository);
                             lock1.unlock();

                       }
                    });

#endif
#endif
                }

            }




}




JNIEXPORT void JNICALL Java_com_example_myapplication1_FristJni_InitRes
(JNIEnv *env,jclass thiz)
{

                static std::string dlc="/sdcard/inception/cropped/frozen_graph_new_quantized.dlc";
                const char* inputfile="/sdcard/inception/cropped/jpeg_list.txt";
                std::string bufferTypeStr="ITENSOR";
                std::string userBufferSourceStr="CPUBUFFER";
                bool usingInitCaching=false;
                //bool execStatus=false;



                enum {UNKNOWN, USERBUFFER_FLOAT, USERBUFFER_TF8, ITENSOR};
                enum {CPUBUFFER, GLBUFFER};


                std::ifstream dlcFile(dlc);
                std::ifstream inputList(inputfile);
                if(!dlcFile||!inputList){
                LOGI("inputfile or dlc is not valid,pleace ensure useful");
                }


                int bufferType;
                      if(bufferTypeStr == "USERBUFFER_FLOAT")
                      {
                      bufferType = USERBUFFER_FLOAT;  //1
                      }
                      else if(bufferTypeStr == "USERBUFFER_TF8")
                      {
                      bufferType = USERBUFFER_TF8; //2
                      }
                      else if(bufferTypeStr=="ITENSOR")
                      {
                      bufferType = ITENSOR; //3
                      }
                      else{
                      LOGI("buffer is not valid");
                      }


                int userBufferSourceType;
                       // CPUBUFFER / GLBUFFER 只支持 USERBUFFER_FLOAT
                       if (bufferType == USERBUFFER_FLOAT)   // 1==1
                       {
                         if( userBufferSourceStr == "CPUBUFFER" )
                         {
                             userBufferSourceType = CPUBUFFER;  //0
                         }
                         else if( userBufferSourceStr == "GLBUFFER" )
                         {
                       #ifndef ANDROID
                             std::cout << "GLBUFFER mode only support android os" << std::endl;
                       #endif
                             userBufferSourceType = GLBUFFER;  //1
                         }
                         else
                         {
                             std::cout<< "Source of user buffer type is not valid"<< std::endl;
                         }
                       }

                //setup udl
                zdl::DlSystem::UDLFactoryFunc udlFunc = sample::MyUDLFactory;
                zdl::DlSystem::UDLBundle udlBundle;udlBundle.cookie=(void*)0xdeadbeaf,udlBundle.func=udlFunc;

                //hold dlc model
                std::unique_ptr<zdl::DlContainer::IDlContainer> container=loadContainerFromFile(dlc);
                if(container==nullptr){
                   LOGI("fail open the contain file");
                }

                bool useUserSuppliedBuffers=(bufferType==USERBUFFER_FLOAT||bufferType==USERBUFFER_TF8);

                zdl::DlSystem::PlatformConfig platformConfig;


                std::vector<std::vector<std::string>> inputs = preprocessInput(inputfile,1);


                for(size_t i=0;i<inputs.size();i++)
                {
                    //LOGI("图片名称:%s",inputs[i][0].c_str());
                    ItemRepository gItemRepository;
                    gItemRepository.flag=i+1;
                    gItemRepository.pic_str=inputs[i];
                    list_pic.push_back(gItemRepository);
                }

                total_Snpe[0] = BuildCPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
                total_Snpe[1] = BuildCPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
                total_Snpe[2] = BuildCPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
                total_Snpe[3] = BuildCPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);

                total_Snpe[4] = (BuildGPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));
                total_Snpe[5] = (BuildGPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));
                total_Snpe[6] = (BuildGPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));
                total_Snpe[7] = (BuildGPU(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));

                total_Snpe[8] = (BuildDSP(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));
                total_Snpe[9] = (BuildDSP(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));
                total_Snpe[10] = (BuildDSP(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));
                total_Snpe[11] = (BuildDSP(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching));



                //dsp5_snpe = BuildDSP(container,udlBundle,useUserSuppliedBuffers,platformConfig,usingInitCaching);
                LOGI("lits_pic size is %d",list_pic.size());
                LOGI("初始化已完成---------------------------");

}


JNIEXPORT jstring JNICALL Java_com_example_myapplication1_FristJni_TestRes
  (JNIEnv *env, jclass thiz){


        time_cost("main",0);

#if 1
        thread tc(output_res,0);
        thread tc2(output_res,1);
        thread tc3(output_res,2);
        thread tc4(output_res,3);
        //thread tc5(output_res,move(cpu5_snpe));
        thread tg(output_res,4);
        thread tg2(output_res,5);
        thread tg3(output_res,6);
        thread tg4(output_res,7);
        //thread tg5(output_res,move(gpu5_snpe));
        thread td(output_res,8);
        thread td2(output_res,9);
        thread td3(output_res,10);
        thread td4(output_res,11);
        //thread td5(output_res,move(dsp5_snpe));

        tc.join();
        tc2.join();
        tc3.join();
        tc4.join();
        //tc5.join();
        tg.join();
        tg2.join();
        tg3.join();
        tg4.join();
        //tg5.join();
        td.join();
        td2.join();
        td3.join();
        td4.join();
        //td5.join();

#endif
        time_cost("main",1);



#if 1
        list_res.sort();
        list<ResItemRepository>::iterator plist;
        LOGI("----------------------------------------result");
        LOGI("----------------------------------------list_len:%d",list_res.size());
        for(plist=list_res.begin();plist!=list_res.end();plist++)
        {
          if(plist->flag){
            int pic_label=plist->flag;
            //zdl::DlSystem::TensorMap outputTensorMap=plist->OTM;
            int location=plist->location;
            float score=plist->score;
            LOGI("The %d pic ,max_locate:%d ,score:%f",pic_label,location,score);

           }
           else{
                LOGI("list_content is None。。。。。。。。。。。。。");
           }

        }

       while(1){
            bool F=!list_res.empty();
            if(F){
                list_res.pop_front();}
            else{
                break;
            }
       }


#endif





    return env->NewStringUTF("DO YOUR SELF");
}






