#include <iostream>
#include <fstream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>
#include <future> 
#include <iomanip>
#include "float.h"
#include "constant_medium.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "texture.h"
#include "aabb.h"
#include "aarect.h"
#include "box.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

hitable* random_scene() {
	int n = 500;
	hitable **list = new hitable*[n + 1];
	texture* checker1 = new checker_texture(new constant_texture(vec3(1, 1, 1)), new constant_texture(vec3(0.5, 0.5, 0.5))); //White diffusive, tinted metal
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian_metal(checker1, 0.0));
	int i = 1;
	float ran;
	for (int a = -27; a < 11; a++) {
		for (int b = -18; b < 11; b++) {
			float choose_mat = (float(rand() % 10000) / 10000);
			do {
				ran = float(rand() % 10000) / 33333;
			} while (ran < 0.1 || ran > 0.3);
			vec3 centre(a + 0.9*(float(rand() % 10000) / 10000), ran, b + 0.9*(float(rand() % 10000) / 10000));
			if ((centre - vec3(4, 0.2, 0)).length() > 0.9f) {
				if (choose_mat < 0.4) {  // moving diffuse
					list[i++] = new moving_sphere(centre, centre + vec3(0.1 * (float(rand() % 10000) / 10000), 0.5 * (float(rand() % 10000) / 10000), 0.15 * (float(rand() % 10000) / 10000)), 0.0, 1.0, ran, new lambertian(new constant_texture(vec3(float(rand() % 10000) / 10000, float(rand() % 10000) / 10000, float(rand() % 10000) / 10000))));
				}
				else if (choose_mat < 0.55) {  // stationary diffuse
					list[i++] = new sphere(centre, ran, new lambertian(new constant_texture(vec3((float(rand() % 10000) / 10000)*(float(rand() % 10000) / 10000), (float(rand() % 10000) / 10000)*(float(rand() % 10000) / 10000), (float(rand() % 10000) / 10000)*(float(rand() % 10000) / 10000)))));
				}
				else if (choose_mat < 0.85) { // metal
					list[i++] = new sphere(centre, ran, new metal(new constant_texture(vec3(float(rand() % 10000) / 10000, float(rand() % 10000) / 10000, float(rand() % 10000) / 10000)),  0.5*(float(rand() % 10000) / 10000)));
				}
				else {  // glass
					list[i++] = new sphere(centre, ran, new dielectric(1.5));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	texture* checker2 = new checker_texture(new constant_texture(vec3(1, 1, 0)), new constant_texture(vec3(1, 0, 0)));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian_metal(checker2, 0.0));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(new constant_texture(vec3(0.7, 0.6, 0.5)), 0.0));

	return new bvh_node(list, i, 0.0, 1.0);
}

hitable* two_spheres() {
	texture* checker1 = new checker_texture(new constant_texture(vec3(1, 1, 1)), new constant_texture(vec3(0.5, 0.5, 0.5)));
	int n = 1;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -10, 0), 10, new fuzz_metal(checker1, 0.0, 0.9));
	texture* checker2 = new checker_texture(new constant_texture(vec3(1, 1, 0)), new constant_texture(vec3(0.2, 0.3, 0.8)));
	list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian_metal(checker2, 0.0));
	return new hitable_list(list, 2);
}

hitable* two_perlin_spheres() {
	texture* pertext1 = new noise_texture(vec3(1,1,0), vec3(0.4,0,0.8), 5, 1);
	texture* pertext2 = new noise_texture(vec3(0, 1, 0), vec3(0, 0, 1), 5, 2);
	texture* checker3 = new checker_texture(new noise_texture(vec3(0, 1, 0), vec3(0, 0, 1), 5, 2), new noise_texture(vec3(1, 1, 0), vec3(0.4, 0, 0.8), 5, 1));
	hitable** list = new hitable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian_metal(checker3, 0.0));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext2));
	return new hitable_list(list, 2);
}

hitable *earth() {
	int nx1, ny1, nn1;
	int nx2, ny2, nn2;
	unsigned char* tex_data1 = stbi_load("Assets/test.bmp", &nx1, &ny1, &nn1, 0);
	unsigned char* tex_data2 = stbi_load("Assets/Turkish.jpg", &nx2, &ny2, &nn2, 0);
/*
	ofstream writer("c:\\Ray-Traced Images\\test.ppm");
	writer << "P3" << endl << nx << endl << ny << endl << "255" << endl;
	for (int j = 0; j < 634; j++) {
		//writer << endl << "j = " << j << endl << endl;
		for (int i = 0; i < 634; i++) {
			writer << int(tex_data[3 * i + 3 * nx*j]) << " " << int(tex_data[3 * i + 3 * nx*j + 1]) << " " << int(tex_data[3 * i + 3 * nx*j + 2]) << endl;
		}
	}
	writer.close();
*/
	material* mat1 = new metal(new image_texture(tex_data1, nx1, ny1), 0.0);
	material* mat2 = new metal(new image_texture(tex_data2, nx2, ny2), 0.0);
	hitable** list = new hitable*[2];
	list[0] = new sphere(vec3(0, -20, 0), 20, mat2);
	list[1] = new sphere(vec3(0, 10, 0), 10, mat1);
	return new hitable_list(list, 2);
}

hitable *simple_light() {
	texture* checker6 = new checker_texture(new constant_texture(vec3(0, 1, 1)), new constant_texture(vec3(1, 1, 1)));
	texture* pertext2 = new noise_texture(vec3(1, 0, 0), vec3(1, 1, 0), 5, 2);
	texture* pertext3 = new noise_texture(vec3(1, 1, 0), vec3(0.4, 0, 0.8), 5, 1);
	texture* pertext4 = new noise_texture(vec3(1, 1, 1), vec3(0, 0, 0), 5, 1);
	texture* pertext5 = new noise_texture(vec3(0, 1, 0), vec3(0, 0, 1), 3, 2);

	int nx1, ny1, nn1;
	int nx2, ny2, nn2;
	int nx3, ny3, nn3;
	int nx4, ny4, nn4;
	unsigned char* tex_data1 = stbi_load("Assets/moon.jpg", &nx1, &ny1, &nn1, 0);
	unsigned char* tex_data2 = stbi_load("Assets/Turkish.jpg", &nx2, &ny2, &nn2, 0);
	unsigned char* tex_data3 = stbi_load("Assets/StainedGlass.jpg", &nx3, &ny3, &nn3, 0);
	unsigned char* tex_data4 = stbi_load("Assets/TARDIS.jpg", &nx4, &ny4, &nn4, 0);
	material* mat1 = new lambertian(new image_texture(tex_data1, nx1, ny1));
	material* mat2 = new diffuse_light(new image_texture(tex_data2, nx2, ny2));
	material* mat3 = new diffuse_light(new image_texture(tex_data3, nx3, ny3));
	material* mat4 = new lambertian(new image_texture(tex_data4, nx4, ny4));

	hitable **list = new hitable*[13];
	int i = 0;
	list[i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext3));
	list[i++] = new sphere(vec3(4, 2, 0), 2, new lambertian(pertext4));
	list[i++] = new sphere(vec3(4, 7, 0), 2, new diffuse_light(pertext2));
	list[i++] = new sphere(vec3(7.5, 0.5, -0.5), 0.5, new diffuse_light(pertext5));
	list[i++] = new xy_rect(2, 10, 0.5, 7, -2, new metal_led_light(checker6, 0.0));
	//list[i++] = new xy_rect(2, 10, 1, 7, -2, new lambertian_metal(checker5, 0.0));
	//list[i++] = new xy_rect(3, 7, 1, 5, -2, new metal(new constant_texture(vec3(1, 1, 1)), 0.0));
	//list[i++] = new xy_rect(3, 7, 1, 5, -2, mat2);
	list[i++] = new sphere(vec3(4, 7, 6), 2, mat1);
	list[i++] = new sphere(vec3(5, 2, 6), 2, mat2);
	//list[i++] = new yz_rect(2, 7, 2, -5, -2, new metal_led_light(checker5, 0.0));
	list[i++] = new yz_rect(3, 7, -1, 11, 2, mat3);
	
	material *off_white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *white = new lambertian(new constant_texture(vec3(1, 1, 1)));
	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(2, 10, 3), off_white), -18), vec3(2, 0, 9));
	hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(0.5, 10, 2.5), white), 15), vec3(4, 0, 1));
	list[i++] = new constant_medium(b1, 1.5, new constant_texture(vec3(1, 0, 0)));
	list[i++] = new constant_medium(b2, 5, new constant_texture(vec3(1, 1, 0)));

	hitable *boundary = new sphere(vec3(8, 1, 3.5), 1, new dielectric(1.5));
	list[i++] = boundary;
	list[i++] = new constant_medium(boundary, 4, new constant_texture(vec3(0.2, 0.4, 0.9)));

	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(1, 2, 1), mat4), 50), vec3(3, 0, 9.5));

	return new hitable_list(list, i);
}

hitable *cornell_box() {
	hitable **list = new hitable*[6];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	texture* checker5 = new checker_texture(new constant_texture(vec3(0, 1, 1)), new constant_texture(vec3(1, 1, 1)));
	material *checkmetal = new lambertian_metal(checker5, 0.0);
	int nx1, ny1, nn1;
	unsigned char* tex_data1 = stbi_load("Assets/Lotto.jpg", &nx1, &ny1, &nn1, 0);
	material* mat1 = new lambertian(new image_texture(tex_data1, nx1, ny1));
	/*
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	*/
	//list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), white);
	//list[i++] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
	//list[i++] = new translate(new rotate_z(new rotate_y(new rotate_x(new box(vec3(0, 0, 0), vec3(165, 165, 165), mat1), 0), 0), 0), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_x(new rotate_z(new translate(new box(vec3(-50, -50, -50), vec3(50, 50, 50), mat1), vec3(0, 0, 0)), 30), 30), vec3(0, 0, 0));
	//list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), checkmetal), 15), vec3(265, 0, 295));
	return new hitable_list(list, i);
}

hitable *cornell_smoke() {
	hitable **list = new hitable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
/*	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
*/	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(0, 1, 1)));
	list[i++] = new constant_medium(b2, 0.001, new constant_texture(vec3(1, 1, 0)));
	return new hitable_list(list, i);
}

hitable *final() {
	int nb = 20;
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	hitable **boxlist2 = new hitable*[10000];
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	int b = 0;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (float(rand() % 10000) / 10000 + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	int nx1, ny1, nn1;
	int nx2, ny2, nn2;
	unsigned char* tex_data1 = stbi_load("Assets/aurora12.jpg", &nx1, &ny1, &nn1, 0);
	unsigned char* tex_data2 = stbi_load("Assets/StainedGlass3.jpg", &nx2, &ny2, &nn2, 0);
	material* mat1 = new diffuse_light(new image_texture(tex_data1, nx1, ny1));
	material* mat2 = new diffuse_light(new image_texture(tex_data2, nx2, ny2));
	list[l++] = new xy_rect(-350, 950, 150, 550, 1000, mat1);
	list[l++] = new xz_rect(23, 523, 140, 540, 554, mat2);
	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(new constant_texture(vec3(0.8, 0.8, 0.9)), 10.0));
	hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.9, 0.2, 0.1)));
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("Assets/Turkish.jpg", &nx, &ny, &nn, 0);
	material *emat = new lambertian(new image_texture(tex_data, nx, ny));
	list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
	texture *pertext = new noise_texture(vec3(1, 1, 0), vec3(0.3, 0.4, 0.8), 0.1, 2);
	list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxlist2[j] = new sphere(vec3(165 * float(rand() % 10000) / 10000, 165 * float(rand() % 10000) / 10000, 165 * float(rand() % 10000) / 10000), 10, white);
	}
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
	return new hitable_list(list, l);
}

hitable *floor() {
	int nb = 20;
	hitable **list = new hitable*[50];
	hitable **boxlist = new hitable*[10000];
	hitable **boxlist2 = new hitable*[10000];
	material *yellow = new lambertian(new constant_texture(vec3(0.73, 0.73, 0)));
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *blue = new lambertian(new constant_texture(vec3(0.12, 0.15, 0.55)));
	material *light = new diffuse_light(new constant_texture(vec3(5, 5, 5)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	int b = 0;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (float(rand() % 10000) / 10000 + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		/*	float choose_mat = (float(rand() % 10000) / 10000);
			if (choose_mat < 0.2) {
				boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
			}
			else if (choose_mat < 0.4) {
				boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), light);
			}
			else if (choose_mat < 0.6) {
				boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), red);
			}
			else if (choose_mat < 0.8) {
				boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), blue);
			}
			else {
				boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), yellow);
			}*/
		}
	}
	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	
	hitable *boundary = new sphere(vec3(160, 100, 145), 100, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.1, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[l++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 430, 165), red), 15), vec3(265, 0, 295));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxlist2[j] = new sphere(vec3(16 * float(rand() % 10000) / 10000, 16 * float(rand() % 10000) / 10000, 1 * float(rand() % 10000) / 10000), 0.01, new dielectric(1.33));
	}
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(278, 278, -790));
	return new hitable_list(list, l);
}

hitable *cornell_final() {
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	texture *pertext = new noise_texture(vec3(1, 1, 0), vec3(0.3, 0.4, 0.8), 0.1, 2);
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("Assets/earthmap.jpg", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex_data, nx, ny));
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	//list[i++] = new sphere(vec3(260, 50, 145), 50,mat);
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(123, 423, 147, 412, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	/*
	hitable *boundary = new sphere(vec3(160, 50, 345), 50, new dielectric(1.5));
	list[i++] = boundary;
	list[i++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
	list[i++] = new sphere(vec3(460, 50, 105), 50, new dielectric(1.5));
	list[i++] = new sphere(vec3(120, 50, 205), 50, new lambertian(pertext));
	int ns = 10000;
	for (int j = 0; j < ns; j++) {
	boxlist[j] = new sphere(vec3(165*drand48(), 330*drand48(), 165*drand48()), 10, white);
	}
	list[i++] =   new translate(new rotate_y(new bvh_node(boxlist,ns, 0.0, 1.0), 15), vec3(265,0,295));
	*/
	hitable *boundary2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), new dielectric(1.5)), -18), vec3(130, 0, 65));
	list[i++] = boundary2;
	list[i++] = new constant_medium(boundary2, 0.2, new constant_texture(vec3(0.9, 0.9, 0.9)));
	return new hitable_list(list, i);
}

hitable *cornell_balls() {
	hitable **list = new hitable*[9];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(5, 5, 5)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	hitable *boundary = new sphere(vec3(160, 100, 145), 100, new dielectric(1.5));
	list[i++] = boundary;
	list[i++] = new constant_medium(boundary, 0.1, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	return new hitable_list(list, i);
}

vec3 colour(const ray& r, hitable* world, int depth) {
	hit_record rec;
	//float MAXFLOAT = numeric_limits<float>::max();
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p, rec);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation*colour(scattered, world, depth + 1);
		}
		else {
			return emitted;
		}
	}
	else {	
		return vec3(0.0, 0.0, 0.0);
	}
}

vec3 getpixel(int x, int nx, int ny, int ns, hitable* world, camera cam) {
	int i = x % nx;
	int j = ny - 1 - x / nx;
	vec3 col(0.0f, 0.0f, 0.0f);
	for (int s = 0; s < ns; s++) {
		float u = (float(i) + float(rand() % 10000) / 10000 - 0.5f) / nx;
		float v = (float(j) + float(rand() % 10000) / 10000 - 0.5f) / ny;
		ray r = cam.get_ray(u, v);
		col += colour(r, world, 0);
	}
	col /= float(ns);
	col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
	int ir = int(255.99f * col[0]);
	int ig = int(255.99f * col[1]);
	int ib = int(255.99f * col[2]);
	return vec3(ir, ig, ib);
}

int main() {
	srand(std::time(0));
	ofstream writer("c:\\Ray-Traced Images\\270.ppm");
	//int nx = 800;
	//int ny = 400;
	int nx = 1920;
	int ny = 1080;
	int ns = 10;
	size_t cores = 8;
	vec3* pixeldata = new vec3[100* nx * ny];

	writer << "P3\n" << nx << " " << ny << "\n255\n";

	hitable* world = final();

	vec3 lookfrom(278, 278, -800); // Cornell box
	vec3 lookat(278, 278, 0);
	float vfov = 35;
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 0.1;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
	auto start = chrono::steady_clock::now();
	size_t max = nx * ny;


	/*hitable* list[8];
	list[0] = new sphere(vec3(0, 2, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.2, 0.3, 0.8)));
	list[2] = new sphere(vec3(-1.5, 0, -1.5), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0));
	list[3] = new sphere(vec3(1.3, 0, -1.2), 0.5, new dielectric(2.5));
	list[4] = new sphere(vec3(0.5, -0.45, -0.55), 0.05, new metal(vec3(0.8, 0, 0), 0.0));
	list[5] = new sphere(vec3(-0.7, -0.4, -0.8), 0.1, new metal(vec3(1, 1, 0), 0.0));
	list[6] = new sphere(vec3(-0.7, 0.5, -2), 0.4, new metal(vec3(0, 1, 0), 0.2));
	list[7] = new sphere(vec3(1.3, -0.3, -3), 0.2, new dielectric(1.7));
	hitable* world = new hitable_list(list, 8);
	*/

	//hitable* world = random_scene();
	//hitable* world = two_spheres();
	//hitable* world = two_perlin_spheres();
	//hitable* world = earth();
	//hitable* world = simple_light();
	//hitable* world = cornell_box();
	//hitable* world = cornell_smoke();
	//hitable* world = cornell_balls();
	//hitable* world = cornell_final();

	//hitable* world = floor();

	//vec3 lookfrom(6, 1.5, 10);
	//vec3 lookfrom(20, 4, 10.5);


	//vec3 lookfrom(0, 0, 100); //Textured-from-files balls
	//vec3 lookat(0, -10, 0);

	/*vec3 lookfrom(20, 3, 6); // Textured spheres and rectangles
	vec3 lookat(0, 4, 2);
	float vfov = 35;
*/
	
/*
	vec3 lookfrom(20, 3.7, 5); //Used for huge spheres scene!
	vec3 lookat(2, 0.25, 0.5);
*/

	//camera cam(lookfrom, lookat, vec3(0, 1, 0), 15, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0); //Used for huge spheres scene!
	//std::size_t cores = std::thread::hardware_concurrency();

	volatile atomic<size_t> count(0);
	vector<future<void>> future_vector;
	while (cores--) future_vector.emplace_back(async([=, &world, &count]()
	{
		while (true)
		{
			size_t index = count++;
			if (index >= max)
				break;
			pixeldata[index] = getpixel(index, nx, ny, ns, world, cam);
		}
	}));
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		float progress = float(count) / max * 100.0f;
		cout.setf(ios::fixed);
		cout.setf(ios::showpoint);
		cout.precision(1);
		cout << "Progress = " << progress << "%" << endl;
		if (count >= max) {
			cout << "Finished! :D"  << endl;
			break;
		}
	}
	auto end = std::chrono::steady_clock::now();
	auto diff = (end - start)/1000;
	cout << std::chrono::duration <double, std::milli>(diff).count() << " seconds" << endl;
	cout << "Now writing to disk..." << endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	for (int x = 0; x < nx * ny; x++) {
		writer << pixeldata[x][0] << " " << pixeldata[x][1] << " " << pixeldata[x][2] << "\n";
	}
	delete[] pixeldata;
	pixeldata = NULL;
	writer.close();
}

