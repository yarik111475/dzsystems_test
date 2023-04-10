#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace bpo=boost::program_options;
using boost::phoenix::ref;
using boost::spirit::qi::_1;
using boost::spirit::qi::int_;
using boost::spirit::qi::char_;
using boost::spirit::qi::double_;

struct point
{
    int index_ {};
    double x_,y_,z_;
    friend std::ostream& operator<<(std::ostream& out, const point& pt){
        out<<pt.index_<<", "<<pt.x_<<", "<<pt.y_<<", "<<pt.z_<<"\n";
        return out;
    }
};

struct element
{
    int index_;
    int pt1_,pt2_,pt3_;
    element(int index,int pt1,int pt2,int pt3)
        :index_{index},pt1_{pt1},pt2_{pt2},pt3_{pt3}{
    }
    friend std::ostream& operator<<(std::ostream& out, const element& el){
        out<<el.index_<<", "<<el.pt1_<<", "<<el.pt2_<<", "<<el.pt3_<<"\n";
        return out;
    }
};

using pointlist_t= std::vector<point>;
using elementlist_t=std::vector<element>;

int main(int argc, char* argv[])
{
    //parse command line arguments
    bpo::options_description desc("Input options");
    desc.add_options()
            ("input,i",bpo::value<std::string>()->required(),"full path to input file")
            ("output,o",bpo::value<std::string>()->required(),"full path to output file");

    bpo::variables_map v_map;
    try{

        bpo::store(bpo::parse_command_line(argc,argv,desc),v_map);
        bpo::notify(v_map);
    }
    catch(std::exception& e){
        std::cout<<e.what()<<std::endl;
        return EXIT_FAILURE;
    }

    //create input file path and check if file exists
    const boost::filesystem::path in_path {v_map.at("input").as<std::string>()};
    if(!boost::filesystem::exists(in_path)){
        const std::string& msg {(boost::format("input file not found, path: %s")
                    % in_path).str()};
        std::cerr<<msg<<std::endl;
        return EXIT_FAILURE;
    }

    pointlist_t pointlist;
    std::string line {};
    std::ifstream in(in_path.string());

    //read file line by line and try to parse them
    while(!in.eof()){
        std::getline(in,line);
        if(line.empty() || boost::starts_with(line,"*")){
            continue;
        }
        boost::erase_all(line," ");

        point pt;
        const auto& begin {line.begin()};
        const auto& end {line.end()};
        const bool& ok {boost::spirit::qi::parse(begin,end,
                      int_[ref(pt.index_)=_1]>>char_(',')
                           >>double_[ref(pt.x_)=_1]>>char_(',')
                           >>double_[ref(pt.y_)=_1]>>char_(',')
                           >>double_[ref(pt.z_)=_1])};

        //if parse success, put point into pointlist
        if(ok){
            pointlist.push_back(pt);
        }
    }

    if(pointlist.empty()){
        const std::string& msg {"pointlist is empty!"};
        std::cerr<<msg<<std::endl;
        return EXIT_FAILURE;
    }

    elementlist_t elementlist;
    int element_index {1};

    //check all points and determine if triangle exists
    for(int i=0;i<pointlist.size();++i){
        for(int j=0;j<pointlist.size();++j){
            const double& ab_x_pow {pow((pointlist.at(i).x_-pointlist.at(j).x_),2)};
            const double& ab_y_pow {pow((pointlist.at(i).y_-pointlist.at(j).y_),2)};
            const double& ab_z_pow {pow((pointlist.at(i).z_-pointlist.at(j).z_),2)};
            const double& ab {sqrt(ab_x_pow + ab_y_pow + ab_z_pow)};

            for(int k=0;k<pointlist.size();++k){
                const double& bc_x_pow {pow((pointlist.at(j).x_-pointlist.at(k).x_),2)};
                const double& bc_y_pow {pow((pointlist.at(j).y_-pointlist.at(k).y_),2)};
                const double& bc_z_pow {pow((pointlist.at(j).z_-pointlist.at(k).z_),2)};
                const double& bc {sqrt(bc_x_pow + bc_y_pow + bc_z_pow)};

                const double& ca_x_pow {pow((pointlist.at(k).x_-pointlist.at(i).x_),2)};
                const double& ca_y_pow {pow((pointlist.at(k).y_-pointlist.at(i).y_),2)};
                const double& ca_z_pow {pow((pointlist.at(k).z_-pointlist.at(i).z_),2)};
                const double& ca {sqrt(ca_x_pow + ca_y_pow + ca_z_pow)};

                if(ab!=bc && bc!=ca && ca!=ab){
                    element el {element_index,i,j,k};
                    elementlist.push_back(el);
                    element_index++;
                }
            }
        }
    }

    //create output file path and output fstream
    const boost::filesystem::path out_path {v_map.at("output").as<std::string>()};
    std::ofstream out(out_path.string());

    //write all nodes
    out<<"* N, X Y Z\n";
    out<<"*Nodes\n";
    for(const point& pt: pointlist){
        out<<pt;
    }

    //write found elements
    out<<"\n\n";
    out<<"Eelemnts\n";
    for(const element& el: elementlist){
        out<<el;
    }

    std::cout<<"all operations finished"<<std::endl;
    std::cout<<"ponts in pointlist: "<<pointlist.size()<<std::endl;
    std::cout<<"elements in elementlist: "<<elementlist.size()<<std::endl;

    return EXIT_SUCCESS;
}
