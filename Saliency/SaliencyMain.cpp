#include "stdafx.h"
void TestGetHC()
{
	CStr wkDir = "G:/SaliencyDataSet/Moseg/";
	CStr inDir = wkDir + "car/", outDir = wkDir + "Saliency/";
	CmFile::Copy2Dir(inDir + "*.jpg", outDir);

	vecS names;
	string sinDir, ext;
	int imgNum = CmFile::GetNamesNE(inDir+"*.jpg", names, sinDir, ext);
	CmFile::MkDir(outDir);
	//vector<CmTimer> timer;
	//for (int f = 0; f < SAL_TYPE_NUM; f++)
	//timer.push_back(CmTimer(SAL_TYPE_DES[f]));

	//disable omp parallel when testing the running efficiency
#pragma omp parallel for 
	for (int i = 0; i < imgNum; i++){
		string name = names[i] + ext;
		//printf("Processing %d/%dth image: %-20s\r", i, imgNum, name.c_str());
		Mat sal, img3f = imread(inDir + name);
		if (img3f.data == NULL){
			printf("Can't load image %s, in %s:%d\n", name.c_str(), __FILE__, __LINE__);
			CmFile::MkDir(outDir + "BadImage");
			CmFile::Copy(inDir + name, outDir + "BadImage\\" + names[i] + ext);
			continue;
		}

		CmFile::Copy(inDir + name, outDir + name);
		img3f.convertTo(img3f, CV_32FC3, 1.0 / 255);

		sal = CmSaliencyRC::GetRC(img3f);
		//timer[f].Stop();
		imwrite(outDir + names[i] + "_" + "RC" + ".png", sal * 255);
	}
	//for (int f = 0; f < SAL_TYPE_NUM; f++)
	//	timer[f].Report();
	printf("Get saliency finished%-40s\n", "");
}
void Evaluate()
{
	CStr wkDir = "G:\\MSRA10K_Imgs_GT\\MSRA10K_Imgs_GT";
	CStr inFolder = "test500", outFolder ="t500out5";
	//CmFile::Copy2Dir(inDir + "*.jpg", outDir);
	
	vecS des;
	des.push_back("RM");
	//des.push_back("HC");  
	//des.push_back("RC");
	//des.push_back("SF"); des.push_back("SalPIF");
	//CmEvaluation::Evaluate(inDir + "*.png", outDir, wkDir + "Results.m", des);
	CmEvaluation::DebugEvalueMask(wkDir, inFolder, outFolder, des, wkDir + "CutRes.m");
}
int EvaluateMain(int argc, char* argv[])
{
	CStr wkDir = argv[1];
	CStr imgFolder = argv[2];
	CStr rstFolder = argv[3];
	CStr methodNam = argv[4];
	vecS des;
	des.push_back(methodNam);
	
	
	//des.push_back("HC");  
	//des.push_back("RC");
	//des.push_back("SF"); des.push_back("SalPIF");
	//CmEvaluation::EvalueMask(wkDir + imgFolder + "\\*.png", wkDir + "\\" + rstFolder+"\\", des, wkDir + "\\Results.m");
	//CmEvaluation::EvalueMaskProposals(wkDir, imgFolder, rstFolder, des, wkDir + "CutRes.m");
	CmEvaluation::DebugEvalueMask(wkDir, imgFolder, rstFolder, des, wkDir + "CutRes.m");
	return 1;
}
int main(int argc, char* argv[])
{	
	return EvaluateMain(argc, argv);
	//TestGetHC();
	//return 0;
	CStr wkDir = "G:\\MSRA10K_Imgs_GT\\MSRA10K_Imgs_GT\\";
	CStr inDir = wkDir + "test500\\", outDir = wkDir + "Test500RRC/";
	//CmFile::Copy2Dir(inDir + "*.jpg", outDir);

	// Saliency detection method pretended in my ICCV 2013 paper http://mmcheng.net/effisalobj/.
	//CmSaliencyGC::Demo(inDir + "*.jpg", outDir); 

	// Saliency detection method presented in PAMI 2014 (CVPR 2011) paper http://mmcheng.net/salobj/.
	//CmSalCut::Demo(inDir + "*.jpg", inDir + "*.png", outDir); 
	CmSaliencyRC::Get(inDir + "*.jpg", outDir);	
	
	//vecS des;
	//des.push_back("GC");  des.push_back("RC");
	////CmEvaluation::Evaluate(inDir + "*.png", outDir, wkDir + "Results.m", des);
	//CmEvaluation::EvalueMask(inDir + "*.png", outDir, "RCC", wkDir + "CutRes.m");

	return 0;
}
