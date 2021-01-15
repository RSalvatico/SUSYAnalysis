// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <istream>
#include <stdio.h>
#include <dirent.h>
#include <vector>

// ROOT includes
#include "TSystem.h"

#include "FitInputBuilder.hh"
#include "SampleTool.hh"
#include "CategoryTool.hh"

using namespace std;

void WriteScript(const string& src_name,
		 const string& log_name,
		 const string& command);

int main(int argc, char* argv[]) {
  int  maxN = 10;
  bool dryRun = false;
  bool doSigFile = false;
  string SigFile = "";
  
  string NtuplePath = "/Users/christopherrogan/Dropbox/SAMPLES/EWKino/NANO/NEW_21_09_20/";
  string OutFile    = "BuildFitInput_output.root";

  bool bprint = false;
  int  year   = 2017;
  bool addBkg  = false;
  bool addSig  = false;
  bool addData = false;
  bool extrahist = false;
  vector<string> proc_to_add;

  CategoryTool CT;
  CategoryList Categories;

  bool setLumi = false;
  double lumi;

  string BuildFitInputCmd;
  
  for(int i = 0; i < argc; i++){
    if(strncmp(argv[i],"--help", 6) == 0){
      bprint = true;
    }
    if(strncmp(argv[i],"-h", 2) == 0){
      bprint = true;
    }
    if(strncmp(argv[i],"-path", 5) == 0){
      i++;
      NtuplePath = string(argv[i]);
      BuildFitInputCmd += "-path " + NtuplePath + " ";
    }
    if(strncmp(argv[i],"-o", 2) == 0){
      i++;
      OutFile = string(argv[i]);
    }
    if(strncmp(argv[i],"--output", 8) == 0){
      i++;
      OutFile = string(argv[i]);
    }
    if(strncmp(argv[i],"-year", 5) == 0){
      i++;
      year = std::atoi(argv[i]);
      BuildFitInputCmd += "-year " + string(argv[i]) + " ";
    }
    if(strncmp(argv[i],"+proc", 5) == 0){
      i++;
      proc_to_add.push_back(argv[i]);
    }
    if(strncmp(argv[i],"++bkg", 5) == 0){
      addBkg = true;
    }
    if(strncmp(argv[i],"++sig", 5) == 0){
      addSig = true;
    }
    if(strncmp(argv[i],"++data", 6) == 0){
      addData = true;
    }
    if(strncmp(argv[i],"++all", 5) == 0){
      addBkg  = true;
      addSig  = true;
      addData = true;
    }
    if(strncmp(argv[i],"+hist", 5) == 0){
      extrahist  = true;
      BuildFitInputCmd += "+hist ";
    }
    if(strncmp(argv[i],"+cat0L", 6) == 0){
      Categories += CT.GetCategories_0L();
      BuildFitInputCmd += "+cat0L ";
    }
    if(strncmp(argv[i],"+cat1L", 6) == 0){
      Categories += CT.GetCategories_1L();
      BuildFitInputCmd += "+cat1L ";
    }
    if(strncmp(argv[i],"+cat2L", 6) == 0){
      Categories += CT.GetCategories_2L();
      BuildFitInputCmd += "+cat2L ";
    }
    if(strncmp(argv[i],"+cat3L", 6) == 0){
      Categories += CT.GetCategories_3L();
      BuildFitInputCmd += "+cat3L ";
    }
    if(strncmp(argv[i],"-lumi", 5) == 0){
      i++;
      setLumi = true;
      lumi = std::stof(argv[i]);
      BuildFitInputCmd += "-lumi " + string(argv[i]) + " ";
    }
    if(strncmp(argv[i],"-maxN", 5) == 0){
      i++;
      maxN = std::max(1, std::stoi(argv[i]));
    }
    if(strncmp(argv[i],"-sigfile", 8) == 0){
      i++;
      doSigFile = true;
      SigFile = argv[i];
    }
    if(strncmp(argv[i],"--dry-run", 9) == 0){
      dryRun = true;
    } 
  }
      
  if((proc_to_add.size() == 0) &&
     (addBkg  == false) &&
     (addSig  == false) &&
     (addData == false))
    bprint = true;

  if(bprint){
    cout << "Usage: " << argv[0] << " [options]" << endl;
    cout << "  Condor submission options:" << endl;
    cout << "   -maxN [number]      maximum number of processes per job" << endl;
    cout << "   -sigfile            signal filename must match this string to be included" << endl;
    cout << "   --dry-run           create output folders and scripts but don't submit" << endl;
    cout << "  options:" << endl;
    cout << "   --help(-h)          print options" << endl;
    cout << "   -path [dest]        path to input ntuples" << endl;
    cout << "   --ouput(-o) [file]  output root file" << endl;
    cout << "   -year [year]        year to process" << endl;
    cout << "   +proc [label]       add processes matching label (can have >1 of these)" << endl;
    cout << "   ++bkg               add all background samples for year" << endl;
    cout << "   ++sig               add all signal samples" << endl;
    cout << "   ++data              add all background samples" << endl;
    cout << "   ++all               add all samples" << endl;
    cout << "   +cat0L              add 0L categories" << endl;
    cout << "   +cat1L              add 1L categories" << endl;
    cout << "   +cat2L              add 2L categories" << endl;
    cout << "   +cat3L              add 3L categories" << endl;
    cout << "   +hist               book 2D histograms also" << endl;
    cout << "   -lumi [lumi]        set luminosity to lumi" << endl;

    return 0;
  }

  cout << "Initializing sample maps from path " << NtuplePath << " for year " << year << endl;
  SampleTool ST(NtuplePath, year);

  ProcessList samples;
  if(addBkg){
    cout << "Adding all background processes" << endl;
    samples += ST.Get(kBkg);
  }
  if(addSig){
    cout << "Adding all signal processes" << endl;
    samples += ST.Get(kSig);
  }
  if(addData){
    cout << "Adding all data for year " << year << endl;
    samples += ST.Get(kData);
  }
  for(int p = 0; p < int(proc_to_add.size()); p++){
    cout << "Adding processes that match \"" << proc_to_add[p] << "\"" << endl;
    samples += ST.Get(proc_to_add[p]);
  }

  // prepare output folder
  string OutputFold = OutFile;
  if(OutputFold.find(".root") != string::npos){
    size_t found = OutputFold.find(".root");
    OutputFold = OutputFold.substr(0,found);
  }

  cout << "* Writing ouput to " << OutputFold << endl;
  gSystem->Exec(("rm -fr "+OutputFold).c_str());
  gSystem->Exec(("mkdir -p "+OutputFold).c_str());

  string SrcFold  = OutputFold+"/src/";
  string LogFold  = OutputFold+"/log/";
  string RootFold = OutputFold+"/root/";
  gSystem->Exec(("mkdir -p "+SrcFold).c_str());
  gSystem->Exec(("mkdir -p "+LogFold).c_str());
  gSystem->Exec(("mkdir -p "+RootFold).c_str());
  
  ofstream condorsubmit;
  condorsubmit.open((OutputFold+"/condor_submit.sh").c_str());
  
  int Nsample = samples.GetN();
  vector<string> procs;
  string iBFICmd;
  int Njob = 0;
  for(int p = 0; p < Nsample; p++){
    Process proc = samples[p];

    if(doSigFile && proc.Type() == kSig){
      bool keep = false;
      int Nfile = ST.NTrees(proc);
      for(int f = 0; f < Nfile; f++){
	string file = ST.FileName(proc, f);
	if(file.find(SigFile) != string::npos)
	  keep = true;
      }
      if(!keep)
	continue;
    }
    
    procs.push_back(proc.Name());
    iBFICmd += "+proc "+proc.Name()+" ";
    cout << "Adding proc " << proc.Name() << endl;
    if(procs.size() >= maxN || p == Nsample-1){
      WriteScript(SrcFold+Form("submit_%d",Njob)+".sh",
		  LogFold+Form("job_%d",Njob)+".log",
		  BuildFitInputCmd+iBFICmd+" -o "+RootFold+Form("BFI_%d.root ", Njob));
      condorsubmit << "condor_submit " << SrcFold << "submit_" << Njob << +".sh" << endl;

      procs.clear();
      iBFICmd = "";
      Njob++;
    }
  }
  condorsubmit.close();

  if(dryRun){
    cout << "Submit condor jobs by typing:" << endl;
    cout << "   source " << OutputFold << "/condor_submit.sh" << endl;
  } else {
    cout << "Submitting condor jobs" << endl;
    gSystem->Exec(("source "+OutputFold+"/condor_submit.sh").c_str());
  }
  
}

void WriteScript(const string& src_name,
		 const string& log_name,
		 const string& command){
  ofstream file;
  file.open(src_name);
  
  file << "universe = vanilla" << endl;
  file << "executable = BuildFitInput.x" << endl;
  file << "getenv = True" << endl;
  file << "use_x509userproxy = true" << endl;
  file << "Arguments = " << command << endl;
  file << "output = " << log_name << ".out" << endl;
  file << "error = "  << log_name << ".err" << endl;
  file << "log = "    << log_name << ".log" << endl;
  file << "Requirements = (Machine != \"red-node000.unl.edu\")" << endl;
  file << "request_memory = 4 GB" << endl;
  file << "queue " << endl;
  file.close();  
}