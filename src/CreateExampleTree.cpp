#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>

int main(int argc, char * argv[]) {
  
  // make sure root-IO knows std::vector
  gROOT->ProcessLine("#include <vector>");

  // declare output
  TFile* file = new TFile("ExampleTree.root","recreate");
  TTree* tree = new TTree("tree","Example Tree");
  Int_t my_int;
  Float_t my_float;
  std::vector<int> my_vector_int;
  std::vector<float> my_vector_float;
  std::vector<std::vector<float> > my_vector_vector_float;
  tree->Branch("my_int",&my_int);
  tree->Branch("my_float",&my_float);
  tree->Branch("my_vector_int",&my_vector_int);
  tree->Branch("my_vector_float",&my_vector_float);
  tree->Branch("my_vector_vector_float",&my_vector_vector_float);

  // generate random data
  Int_t nEvents = 10e6;
  for (Int_t i = 0; i < nEvents; ++i) {

    // clear vectors
    my_vector_int.clear();
    my_vector_float.clear();
    my_vector_vector_float.clear();

    // fill variables/vectors
    std::vector<float> another_vector_float;
    my_int = gRandom->Poisson(10.0);
    my_float = gRandom->Gaus(50.0,10.0);
    for (Int_t j = 0; j < my_int; ++j) {
      my_vector_int.push_back(gRandom->Poisson(my_float));
      my_vector_float.push_back(gRandom->Gaus(my_float/2.,5.0));
      another_vector_float.push_back(gRandom->Gaus(my_float/3.,2.0));
    }
    my_vector_vector_float.push_back(my_vector_float);
    my_vector_vector_float.push_back(another_vector_float);

    // fill tree
    tree->Fill();

  }

  // save to file
  file->Write();
  file->Close();

  return 0;

} 
