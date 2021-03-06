#include <iostream>
#include <cstring>

#include "../src/WaliPkgCmd.h"
#include "../src/log.h"
int main() {

    WaliPkgCmd p;

    std::vector<std::string> v;
    v.emplace_back("zip");
    v.emplace_back("zfs-linux");
    auto res = p.get_source_package_info(v);

    for(auto item : res){
        std::cout<<item.get_name()<<" "<<item.get_version()<<std::endl;
        auto a = item.get_package_name_list();
        for(auto package : a){
            std::cout<<"\t"<<package<<std::endl;
        }
    }

    return 0;
    //设置进度更新时的回调函数
    p.setProgressCallback([](WaliSchedule progress, TaskType type){
        std::string str;
        if(type == TASK_FIND_PACKAGE)
            str = "查找进度：";
        else if(type == TASK_INSTALL_PACKAGE)
            str = "安装进度：";
        else if(type == TASK_REMOVE_PACKAGE)
            str = "卸载进度：";
        else if(type == TASK_REFRESH_CACHE)
            str = "更新缓存：";
        std::cout<<"\r"<<str<<progress.get_percentage()<<"% speed:"<<progress.get_speed()<<"Mb/s"<<std::flush;
        ;
//        <<"剩余时间："<<progress.get_remaining_time()<<std::flush;
//        <<"包："<<progress.get_package().get_package_name()<< std::flush; //flush作用是及时输出（清楚缓冲区）
    });

    auto repos = p.get_repo_list();
    for(auto repo : repos){
        log(repo.get_repo_id());
    }

    //更新缓存
    p.refresh_cache(false);

    if(!p.error().empty()){
        return 0;
    }

    //生成一个要查找包的包名列表
    std::vector<std::string> values;
    values.emplace_back("gimp");

    //获取查找结果
    std::cout<<"\n正在搜索...\n";
    auto pkg_list = p.find_packages_based_on_names(values);

    //筛选
    std::vector<WaliPkg> install_list;
    for(auto pkg : pkg_list){
        //将架构为x86_64的未安装的软件包加入安装列表
        if(std::strcmp(pkg.get_package_arch().c_str(),"amd64") == 0 && !pkg.is_installed()){
            install_list.emplace_back(pkg);
        }
    }

    //根据筛选结果提示和安装
    if(install_list.empty()){
        std::cout<<"\n您没有任何需要安装的软件包。\n";
    } else{
        std::cout<<"\n将安装以下软件包：\n";
        for(auto pkg : install_list){
            std::cout<<pkg.get_package_id()<<"\n\t"<<pkg.get_package_summary()<<std::endl;
        }
        std::cout<<"开始安装...\n";
        //安装所有软件包
        p.install_packages(install_list);
//        p.remove_packages(install_list);
        std::cout<<"\n安装完成"<<std::endl;
    }
    return 0;
}
