#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>

#define R 6371
#define M_PI 3.14159265358979323846

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;


double toRadians(double degree) {
    return degree * M_PI / 180.0;
}

double haversineDistance(City city1, City city2) {
    double lat1 = toRadians(city1.latitude);
    double lon1 = toRadians(city1.longitude);
    double lat2 = toRadians(city2.latitude);
    double lon2 = toRadians(city2.longitude);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}