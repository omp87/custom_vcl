#include "vcl/CustomVCL.h"
#include <opencv2/imgproc.hpp>

int main(int argc, char* argv[])
{

    //create IPC structures for communicating between processes
    key_t key_ctl_host_remote;
    key_ctl_host_remote = ftok("keyfile", 60);
    int msgid_ctl_host_remote = msgget(key_ctl_host_remote, 0666 | IPC_CREAT);
    message message_ctl_host_remote;

    key_t key_data_host_remote;
    key_data_host_remote = ftok("keyfile", 61);
    int shmid_data_host_remote = shmget(key_data_host_remote,SHARED_IMAGE_BUFFER_SIZE,0666|IPC_CREAT);
    uint8_t *image_buffer = (uint8_t*) shmat(shmid_data_host_remote,(void*)0,0);

    key_t key_ctl_remote_host;
    key_ctl_remote_host = ftok("keyfile", 62);
    int msgid_ctl_remote_host = msgget(key_ctl_remote_host, 0666 | IPC_CREAT);;
    message message_ctl_remote_host;

    while(true)
    {    
        msgrcv(msgid_ctl_host_remote, &message_ctl_host_remote,sizeof(message) , 1, 0);

        cv::Mat* in_image = new cv::Mat(message_ctl_host_remote.data_rows, message_ctl_host_remote.data_cols, message_ctl_host_remote.data_type);
        memcpy((uint8_t*) &(in_image->data[0]), image_buffer, message_ctl_host_remote.data_image_size);

        char* json_string_char = new char[message_ctl_host_remote.data_json_size];
        memcpy(&(json_string_char[0]), &(image_buffer[message_ctl_host_remote.data_image_size]), message_ctl_host_remote.data_json_size);
        std::string json_string(json_string_char);
        std::cout << json_string << std::endl;

        //Manipulate Image
        cv::inRange(*in_image, cv::Scalar(0,0,0), cv::Scalar(255,255,255), *in_image);
        cv::resize(*in_image, *in_image, cv::Size(int(in_image->cols/8), int(in_image->rows/8)));

        //Copy image back into shared memory
        size_t in_image_size = in_image->total() * in_image->elemSize();
        memcpy(&(image_buffer[0]), &(in_image->data[0]), in_image_size);

        //Send Response back to host
        message_ctl_remote_host.message_type = 1;
        message_ctl_remote_host.data_rows = in_image->rows;
        message_ctl_remote_host.data_cols = in_image->cols;
        message_ctl_remote_host.data_type = in_image->type();
        message_ctl_remote_host.data_image_size = in_image_size;
        message_ctl_remote_host.data_json_size = 0;

        int msg_send_result = msgsnd(msgid_ctl_remote_host, &message_ctl_remote_host, sizeof(message), 0);
        if(msg_send_result < 0)
        { }

        //Free allocated memory
        delete in_image;
        delete json_string_char;
    }

    //Free shared IPC components
    msgctl(msgid_ctl_host_remote, IPC_RMID, NULL);
    shmdt(image_buffer);
    shmctl(shmid_data_host_remote,IPC_RMID,NULL);
   return 0;
}

void custom_vcl_function(VCL::Image& img, const Json::Value& op)
{
    cv::Mat tmp = img.get_cvmat(true);
    if(op.get("custom_function_type", 0).asString() == "hsv_threshold")
    { 
        cv::cvtColor(tmp, tmp, cv::COLOR_RGB2HSV);
        cv::inRange(tmp, cv::Scalar(op.get("h0", -1).asInt(), op.get("s0", -1).asInt(), op.get("v0", -1).asInt()), cv::Scalar(op.get("h1", -1).asInt(),op.get("s1", -1).asInt(),op.get("v1", -1).asInt()), tmp);
        img.deep_copy_cv(tmp);
    }
    else
    {
        img.deep_copy_cv(tmp);
    }

}
