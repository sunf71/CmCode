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
	int fId = atoi(argv[1]);
	CStr wkDir = argv[2];
	CStr imgFolder = argv[3];
	CStr rstFolder = argv[4];
	CStr methodNam = argv[5];
	vecS des;
	des.push_back(methodNam);
	
	switch (fId)
	{
	case 0:
		CmEvaluation::EvalueMask(wkDir + imgFolder + "\\*.png", wkDir + "\\" + rstFolder + "\\", des, wkDir + "\\Results.m");
		break;
	case 1:
		CmEvaluation::DebugEvalueMask(wkDir, imgFolder, rstFolder, des, wkDir + "CutRes.m");
		break;
	case 2:
		CmEvaluation::EvalueMaskProposals(wkDir, imgFolder, rstFolder, des, wkDir + "CutRes.m");
		break;

	default:
		break;
	}
	//des.push_back("HC");  
	//des.push_back("RC");
	//des.push_back("SF"); des.push_back("SalPIF");
	//CmEvaluation::EvalueMask(wkDir + imgFolder + "\\*.png", wkDir + "\\" + rstFolder+"\\", des, wkDir + "\\Results.m");
	//CmEvaluation::EvalueMaskProposals(wkDir, imgFolder, rstFolder, des, wkDir + "CutRes.m");
	//CmEvaluation::DebugEvalueMask(wkDir, imgFolder, rstFolder, des, wkDir + "CutRes.m");
	return 1;
}
void ChooseWeight()
{
	FILE * pFile(NULL);
	struct Para
	{
		int id;
		float obj;
		float fill;
		float size;
	
	};
	struct ParaSet
	{
		std::string name;
		std::vector<Para> paras;
		int maxId;
	};
	errno_t err = fopen_s(&pFile, "data.txt", "rt");
	char name[20];
	int id(0),max(0);
	float fill(0), size(0), obj(0);
	std::vector<ParaSet> paraSets;
	
	char buffer[512];
	while (fgets(buffer,512,pFile))
	{
		strcpy_s(name, buffer);
		std::vector<Para> paras;
		while (1)
		{
			
			fgets(buffer, 512, pFile);
			if (strlen(buffer) > 10)
			{
				sscanf_s(buffer, "%d\t%f\t%f\t%f\n%d", &id, &fill, &size, &obj);
				Para para;
				para.id = id - 2;
				para.fill = fill;
				para.size = size;
				para.obj = obj;
				paras.push_back(para);
			}
			else
			{
				
				sscanf_s(buffer, "%d", &max);
				ParaSet set;
				set.maxId = max - 2;
				set.paras = paras;
				set.name = std::string(name);
				paraSets.push_back(set);
				break;
			}
		}
		
		
		
		
	}
	
	fclose(pFile);
	float step = 0.05;
	float wf, ws, wo;
	float maxHitRate(0);
	float maxWf, maxWs, maxWo;
	struct proposal
	{
		int id;
		float score;
	};
	struct propCMP
	{
		bool operator()(proposal& p1, proposal& p2)
		{
			return p1.score > p2.score;
		}
	};
	char wkpath[200] = "e:\\";
	for ( wf = 0; wf < 1; wf += step)
	{
		for ( ws = 0; ws < 1; ws += step)
		{
			wo = 1 - wf - ws;
			float count(0);
			
			for (size_t i = 0; i < paraSets.size(); i++)
			{
				std::vector<proposal> proposals;
				float maxWeight(0);
				int maxId(0);
				paraSets[i].name;
				char imgName[200];
				cv::Mat result;
				for (size_t j = 0; j < paraSets[i].paras.size(); j++)
				{

					Para para = paraSets[i].paras[j];
					
					float weight = para.fill*wf + para.obj*wo + para.size*ws;
					
					proposal p;
					p.id = j;
					p.score = weight;
					if (weight > maxWeight)
					{
						maxWeight = weight;
						maxId = j;
					}
					proposals.push_back(p);

				}
				/*if (maxId == paraSets[i].maxId)
					count++;*/
				std::sort(proposals.begin(), proposals.end(), propCMP());
				for (size_t p = 0; p < proposals.size(); p++)
				{
					sprintf(imgName, "%s%s\\dSaliency_%2d_%2d_%2d.png", wkpath, paraSets[i].name, para.id + 2, (int)(para.fill * 100), (int)(para.size * 100), (int)(para.obj * 100));
					cv::Mat sal = cv::imread(imgName, -1);
					if (result.empty())
						result = cv::Mat::zeros(sal.size(), CV_32F);
					if (sal.channels() == 3)
						cv::cvtColor(sal, sal, CV_BGR2GRAY);
					float weight = para.fill*wf + para.obj*wo + para.size*ws;
					cv::addWeighted(sal, weight, result, 1, 0, result, CV_32F);
				}

				if (proposals[0].id == maxId)
					count++;

			}
			
			float hitRate = count / paraSets.size();
			if (hitRate > maxHitRate)
			{
				maxHitRate = hitRate;
				maxWf = wf;
				maxWo = wo;
				maxWs = ws;
			}
		}
	}
	
	
	std::cout << maxWf << "," << maxWo << "," << maxWs << " hitRate " << maxHitRate << "\n";
}
int main(int argc, char* argv[])
{	
	ChooseWeight();
	return 0;
	//return EvaluateMain(argc, argv);
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
