#ifndef NeventTool_h
#define NeventTool_h

#include <iostream>
#include <string>
#include <map>

#include "TFile.h"
#include "TTree.h"

class NeventTool {

public:
  NeventTool();
  virtual ~NeventTool();

  void BuildMap(const std::string& rootfile);
  void BuildFilterEffMap(const std::string& rootfile);

  double GetNevent_BKG(const std::string& dataset, const std::string& filetag) const;
  double GetNevent_SMS(const std::string& dataset, const std::string& filetag, int MP, int MC) const;
  double GetNweight_BKG(const std::string& dataset, const std::string& filetag) const;
  double GetNweight_SMS(const std::string& dataset, const std::string& filetag, int MP, int MC) const;
  double GetFilterEff(const std::string& dataset, const std::string& filetag, int lumiblock = -1) const;

private:
  static std::map<std::pair<std::string,std::string>,double> m_Label2Nevent_BKG;
  static std::map<std::pair<std::string,std::string>,double> InitMap_Nevent_BKG();
  static std::map<std::pair<std::string,std::string>,double> m_Label2Nweight_BKG;
  static std::map<std::pair<std::string,std::string>,double> InitMap_Nweight_BKG();
  static std::map<std::pair<std::string,std::string>,std::map<std::pair<int,int>,double> > m_Label2Nevent_SMS;
  static std::map<std::pair<std::string,std::string>,std::map<std::pair<int,int>,double> > InitMap_Nevent_SMS();
  static std::map<std::pair<std::string,std::string>,std::map<std::pair<int,int>,double> > m_Label2Nweight_SMS;
  static std::map<std::pair<std::string,std::string>,std::map<std::pair<int,int>,double> > InitMap_Nweight_SMS();

  static std::map<std::string,std::map<int,double> > InitMap_FilterEff();
  static std::map<std::string,std::map<int,double> > m_Label2FilterEff;

  std::string m_RootFile;
  TFile* m_File;
  TTree* m_Tree;
  std::string m_RootFile_FE;
  TFile* m_File_FE;
  TTree* m_Tree_FE;

  int m_lumiblock;
  double m_efficiency;
  
  double m_Nevent;
  double m_Nweight;
  std::string* m_dataset;
  std::string* m_filetag;
  int m_MP;
  int m_MC;
  TBranch* b_m_lumiblock;  
  TBranch* b_m_efficiency; 
  TBranch* b_m_Nevent;  
  TBranch* b_m_Nweight; 
  TBranch* b_m_dataset;
  TBranch* b_m_filetag;
  TBranch* b_m_MP;
  TBranch* b_m_MC;

  void Initialize_BKG(const std::string& dataset, const std::string& filetag) const;
  void Initialize_SMS(const std::string& dataset, const std::string& filetag) const;
  void Initialize_FE(const std::string& dataset, const std::string& filetag) const;
  
};

#endif



