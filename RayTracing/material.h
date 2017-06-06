#pragma once
struct Material{
	double ambient[4];
	double diffuse[4];
	double specular[4];
	double shininess;
	Material(){};
	Material(double* amb, double* dif, double* spe, double shi){
		for (int i=0;i<4;i++) ambient[i]=amb[i];
		for (int i=0;i<4;i++) diffuse[i]=dif[i];
		for (int i=0;i<4;i++) specular[i]=spe[i];
		shininess = shi;
	}
};

double metal_amb[4]={0.1f,0.1f,0.1f,1.0f};
double metal_dif[4]={0.2f,0.2f,0.2f,1.0f};
double metal_spe[4]={0.7f,0.7f,0.7f,1.0f};
double metal_shi = 128;

Material metal = Material(metal_amb, metal_dif, metal_spe, metal_shi);
double default_amb[4]={0.2f,0.2f,0.2f,1.0f};
double default_dif[4]={0.8f,0.8f,0.8f,1.0f};
double default_spe[4]={0.0f,0.0f,0.0f,0.0f};
double default_shi = 0;
Material default_material = Material(default_amb, default_dif, default_spe, default_shi);

double gold_amb[4]={0.24725f,0.1995f,0.0745f,1.0f};
double gold_dif[4]={0.75164f,0.60648f,0.22648f,1.0f};
double gold_spe[4]={0.628281f,0.555802f,0.366065f,1.0f};
double gold_shi = 128*0.4f;
Material gold = Material(gold_amb, gold_dif, gold_spe, gold_shi);

double silver_amb[4]={0.19225f,0.19225f,0.19225f,1.0f};
double silver_dif[4]={0.50754f,0.50754f,0.50754f,1.0f};
double silver_spe[4]={0.508273f,0.508273f,0.508273f,1.0f};
double silver_shi = 128*0.4f;
Material silver = Material(silver_amb, silver_dif, silver_spe, silver_shi);

double chrome_amb[4]={0.25f,0.25f,0.25f,1.0f};
double chrome_dif[4]={0.4f,0.4f,0.4f,1.0f};
double chrome_spe[4]={0.774597f,0.774597f,0.774597f,1.0f};
double chrome_shi = 128*0.6f;
Material chrome = Material(chrome_amb, chrome_dif, chrome_spe, chrome_shi);

double emerald_amb[4]={0.0215f,0.1745f,0.0215f,1.0f};
double emerald_dif[4]={0.07568f,0.61424f,0.075568f,1.0f};
double emerald_spe[4]={0.633f,0.727811f,0.633f,1.0f};
double emerald_shi = 128*0.6f;
Material emerald = Material(emerald_amb, emerald_dif, emerald_spe, emerald_shi);

double plastic_amb[4]={0.0f,0.1f,0.06f,1.0f};
double plastic_dif[4]={0.0,0.5098f,0.5098f,1.0f};
double plastic_spe[4]={0.5019f,0.5019f,0.5019f,1.0f};
double plastic_shi = 128*0.95f;
Material plastic = Material(plastic_amb, plastic_dif, plastic_spe, plastic_shi);

double ruby_amb[4]={0.1745f,0.01175f,0.01175f,1.0f};
double ruby_dif[4]={0.61424f,0.04136f,0.04136f,1.0f};
double ruby_spe[4]={0.727811f,0.626959f,0.626959f,1.0f};
double ruby_shi = 128*0.6f;
Material ruby = Material(ruby_amb, ruby_dif, ruby_spe, ruby_shi);

double yellow_rubber_amb[4]={0.05f,0.05f,0.00f,1.0f};
double yellow_rubber_dif[4]={0.5f,0.5f,0.4f,1.0f};
double yellow_rubber_spe[4]={0.7f,0.7f,0.04f,1.0f};
double yellow_rubber_shi = 128*0.78125f;
Material yellow_rubber = Material(yellow_rubber_amb, yellow_rubber_dif, yellow_rubber_spe, yellow_rubber_shi);

double yellow[] = {1.0f, 1.0f, 0.0f};
double red[] = {1.0f, 0.0f, 0.0f};
double green[] = {0.0f, 1.0f, 0.0f};
double blue[] = {0.0f, 0.0f, 1.0f};
double pink[] = {1.0f, 0.1f, 0.5f};
double yellowgreen[] = {0.25f, 0.8f, 0.2f};
double skyblue[] = {0.0f, 1.0f, 1.0f};
double gray[] = {0.9f, 0.9f, 0.9f};
double blackgray[] = {0.3f, 0.3f, 0.3f};
