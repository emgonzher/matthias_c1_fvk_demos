// /// ===== WORKING - use this to define print_function
// #include <sstream>
// #include <fstream>
// #include <iostream>
// #include <string>

// int main() 
// {
// std::ifstream ifs("example_read.dat");

// std::string line;

//  unsigned i = 0;

// while (i<50) // read one line from ifs
// {
//     std::getline(ifs, line);
    
//     // // ===============
//     // std::istringstream iss(line); // access line as a stream

//     // // we only need the first two columns
//     // float column1;
//     // float column2;

//     // iss >> column1 >> column2; // no need to read further

//     // // do what you will with column2
//     // std::cout << column2 << std::endl;
//     // // ===================

//     // print everything
//     // ===============
//     std::cout << line << std::endl;
//     // ===================

//     i += 1;

// }
// }
// /// =====



/// ==== Trying print_column(filename,ncolumn) function
// #include <sstream>
// #include <fstream>
// #include <iostream>
// #include <string>

// //int main() 
// void print_column(std::string filename)
// //void print_column()
// {
// std::ifstream ifs(filename);
// //std::ifstream ifs("example_read.dat");

// std::string line;

//  //unsigned i = 0;

// while (std::getline(ifs, line)) // read one line from ifs
// {
//     //std::getline(ifs, line);
    
//     // ===============
//     std::istringstream iss(line); // access line as a stream

//     // we only need the first two columns
//     float c1, c2, c3, c4, c5, c6, c7, 
//           c8, c9, c10, c11, c12, c13, c14;

//     iss >> c1 >> c2 >> c3 >> c4 >> c5 
//         >> c6 >> c7 >> c9 >> c10 >> c11 
//         >> c12 >> c13 >> c14; // read until needed

//     // show columns you need
//     std::cout << c3  << " "
//               << c9  << " " 
//               << c5  << " "
//               << c12 << std::endl;
//     // ===================

//     // print everything
//     // ===============
//     // std::cout << line << std::endl;
//     // ===================

//     //print everything but in outfile
//     // ====================
    

//     //i += 1;

// }
// }

// int main()
// {
//  print_column("critical_sol_m6_50.dat");
// }

/// ===============================================

// #include <stdio.h>

// int main() {
//     FILE *fptr = fopen("example2.dat", "r");
    
//     // Reading the file data using fgets() in the
//     // form of a block of size 30 bytes
//     char buff[30];
//     unsigned i = 0;
//     double n1, n3;
//     while (i < 100)
//     {
//     fgets(buff, sizeof(buff), fptr);
//     fscanf(fptr,"%lf %*lf %lf %*lf",&n1,&n3);
//     printf("%lf", n1);
//     //printf("%s", buff);
//     i += 1;
//     }
//     fclose(fptr);
//     return 0;
// }

// ===============================================================
// Trying to include file reading in WORKING code
// ===============================================================

// #include<iostream>
// #include<cmath>
// #include <fstream>
// using std::ofstream;
// using std::ifstream;
// using std::endl;

// int main()
// {

//  double ampl=0.1;
//  unsigned N=5;
//  double epsilon=0.01;
 
//  unsigned nr=40;
//  unsigned nphi=105;

//  ofstream outdata;
//  ifstream data2read("test_sol_P3.000_Eta1.410e+05.dat");

//  std::string name;

//  fgets(data2read,name);
//  double a;
//  double b;
//  double c;

//  fscanf (data2read,"%d %*d %d",&a,&b,&c);
//  std::cout << a << endl;

//   return 0;
// }


// #include <iostream>
// #include <fstream>
// using namespace std;
// int main ()
// {
//     int data[6],a,b,c,d,e,f; 
//     ifstream myfile; 
//     myfile.open ("a.txt");
//     for (int i=0;i<<6;i++) 
//     { 
//         myfile>>data[i]; 
//     } 
//     myfile.close(); 
//     a=data[0]; 
//     b=data[1]; 
//     c=data[2]; 
//     d=data[3]; 
//     e=data[4]; 
//     f=data[5]; 
//     cout<<a<<"\t"<<b<<"\t"<<c<<"\t"<<d<<"\t"<<e<<"\t"<<f<<"\n"; 
//     system ("pause"); 
//     return 0;
// }



// ====
//  outdata.open("example2.dat");

//  //std::cout << "ZONE I=" << nphi << ", J=" << nr << std::endl;
//  outdata << "ZONE I=" << nphi << ", J=" << nr << std::endl;

//  for (unsigned i=0;i<nr;i++)  
//   {
//    getline(data2read, mydata)
//    double r=double(i)/double(nr-1);
//    for (unsigned j=0;j<nphi;j++)
//     {
//      double phi=2.0*4.0*atan(1.0)*double(j)/double(nphi-1);
//      outdata << r*cos(phi) << " "
//                << r*sin(phi) << " " 
//                << ampl*r*r+r*r*r*r*epsilon*cos(double(N)*phi) << " "
//                << endl;
//     }
//   }
//   outdata.close();
//   return 0;
//  }






// ==================================================================
// WORKING
// ==================================================================

// #include<iostream>
// #include<cmath>
// #include <fstream>
// using std::ofstream;
// using std::endl;

// int main()
// {

//  double ampl=0.1;
//  unsigned N=5;
//  double epsilon=0.01;
 
//  unsigned nr=50;
//  unsigned nphi=105;

//  ofstream outdata;

//  outdata.open("test_sol_phi.dat");

//  outdata << "ZONE I=" << nr << ", J=" << nphi << std::endl;


//  for (unsigned i=0;i<nphi;i++)  
//   {
//    double phi=2.0*4.0*atan(1.0)*double(i)/double(nphi-1);
//    for (unsigned j=0;j<nr;j++)
//     {
//       double r=double(j)/double(nr-1);
//       outdata << r*cos(phi) << " "
//                 << r*sin(phi) << " " 
//                 << ampl*r*r+r*r*r*r*epsilon*cos(double(N)*phi) << " "
//                 << -2.0*ampl*r*r-r*r*r*r*epsilon*cos(double(N)*phi) 
//                 << endl;
//     }
//   }
//   outdata.close();
//   return 0;
//  }


 // ======= not using function... but maybe working

#include<iostream>
#include<cmath>
#include <fstream>
#include <sstream>
#include <fstream>
#include <string>
using std::ofstream;
using std::endl;

int main()
{

 double ampl=0.1;
 unsigned N=5;
 double epsilon=0.01;
 
 unsigned nr=50;
 unsigned nphi=105;
 unsigned npts = 5;

 ofstream outdata;

 outdata.open("RESLT_axi/axi_surface_P2.dat");

 outdata << "ZONE I=" << nr*npts << ", J=" << nphi << std::endl;

 for (unsigned i=0;i<nphi;i++)  
  {
    std::ifstream ifs("RESLT_axi/axi_sol_P2.000_Eta1.410e+05.dat");

    std::string line;
   double phi=2.0*4.0*atan(1.0)*double(i)/double(nphi-1);
   for (unsigned j=0;j<nr*npts;j++)
    {
     double r=double(j)/double(nr*npts-1);
     std::getline(ifs, line);

     // ===============
    std::istringstream iss(line); // access line as a stream

    // columns to read
    float c1, c2, c3, c4, c5, c6, c7, 
          c8, c9, c10, c11, c12, c13, c14;

    iss >> c1 >> c2 >> c3 >> c4 >> c5 
        >> c6 >> c7 >> c8 >> c9 >> c10 
        >> c11 >> c12 >> c13 >> c14; // read until needed
    // ===================
     outdata << r*cos(phi) << " "
               << r*sin(phi) << " " 
               << c3 << " "  // u - V1
               << c9 << " "  // w - V2
               << c5 << " "  // u_pert - V3
               << c12 << " " // w_pert - V4
               << c5*cos(double(N)*phi) << " " // u_pert*cos(Nphi) - V5
               << c12*cos(double(N)*phi) << " " // w_pert*cos(Nphi) - V6
               << endl;
    }
  }
  outdata.close();
  return 0;
 }

 //
