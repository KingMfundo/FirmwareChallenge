//By Mfundo Mbambo

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<string.h>
#include <float.h>
#include <omp.h> // Include OpenMP header for parallel processing
#include <sys/time.h> // Include header for timing

// Define a structure to store vehicle position data
struct VehiclePosition {
    int positionId;
    char registration[50];
    float latitude;
    float longitude;
    unsigned long long timestamp;
};

// Function to calculate the distance between two points using Haversine formula
double haversineDistance(float lat1, float lon1, float lat2, float lon2) {
    // Convert latitude and longitude from degrees to radians
    double lat1Rad = lat1 * M_PI / 180.0;
    double lon1Rad = lon1 * M_PI / 180.0;
    double lat2Rad = lat2 * M_PI / 180.0;
    double lon2Rad = lon2 * M_PI / 180.0;

    // Haversine formula
// Calculate the difference in latitude and longitude between two points in radians
double dLat = lat2Rad - lat1Rad;    // Difference in latitude
double dLon = lon2Rad - lon1Rad;    // Difference in longitude

// Haversine formula to calculate the distance between two points on the Earth's surface
// based on their latitude and longitude
double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1Rad) * cos(lat2Rad) * sin(dLon / 2) * sin(dLon / 2);
double c = 2 * atan2(sqrt(a), sqrt(1 - a));    // Calculate the central angle using atan2 function

const double earthRadius = 6371000.0; // Earth's radius in meters (approximate value)
double distance = earthRadius * c;   // Calculate the distance using the Haversine formula

return distance;    // Return the calculated distance between the two points in meters

}

int main() {
    // Open the "VehiclePositions.dat" file in binary read mode
    FILE* file = fopen("VehiclePositions.dat", "rb");

    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1; // Return an error code to indicate the failure
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("File size: %ld bytes\n", fileSize);

    // Calculate the number of records in the file
   int numRecords = fileSize / sizeof(struct VehiclePosition);
    printf("Number of records in the file: %d\n", numRecords);

    // Make sure the file is not too large for our array
    if (numRecords > 2000000) {
        printf("Error: The file contains too many records.\n");
        fclose(file);
        return 1;
    }

    // Allocate memory for the vehicleData dynamically
    struct VehiclePosition* vehicleData = (struct VehiclePosition*)malloc(numRecords * sizeof(struct VehiclePosition));

    // Check if memory allocation was successful
    if (vehicleData == NULL) {
        printf("Memory allocation error.\n");
        fclose(file);
        return 1;
    }

    // Read the data into memory
    fread(vehicleData, sizeof(struct VehiclePosition), numRecords, file);

    // Define the 10 given coordinates as an array of struct VehiclePosition
    struct VehiclePosition givenCoordinates[10] = {
        { 1, "", 34.544909, -102.100843, 0 },    // Given Coordinate 1
        { 2, "", 32.345544, -99.123124, 0 },     // Given Coordinate 2
        { 3, "", 33.234235, -100.214124, 0 },    // Given Coordinate 3
        { 4, "", 35.195739, -95.348899, 0 },     // Given Coordinate 4
        { 5, "", 31.895839, -97.789573, 0 },     // Given Coordinate 5
        { 6, "", 32.895839, -101.789573, 0 },    // Given Coordinate 6
        { 7, "", 34.115839, -100.225732, 0 },    // Given Coordinate 7
        { 8, "", 32.335839, -99.992232, 0 },     // Given Coordinate 8
        { 9, "", 33.535339, -94.792232, 0 },     // Given Coordinate 9
        { 10, "", 32.234235, -100.222222, 0 }    // Given Coordinate 10
    };

    // Process data in chunks of 1000 records
    const int chunkSize = 1000;
    struct VehiclePosition* chunkData = (struct VehiclePosition*)malloc(chunkSize * sizeof(struct VehiclePosition));

    // Check if memory allocation was successful
    if (chunkData == NULL) {
        printf("Memory allocation error.\n");
        free(vehicleData); // Free previously allocated memory
        fclose(file);
        return 1;
    }

    // Initialize arrays to store closest position IDs
    int closestPositions[10] = {0}; // Stores the closest position ID for each given coordinate

    // Start timing
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Process the data sequentially
   // Iterate through the vehicleData array in chunks, where each chunk contains 'chunkSize' records
// The loop starts from 0 and increments 'i' by 'chunkSize' at each iteration
 #pragma omp parallel for
for (int i = 0; i < numRecords; i += chunkSize) {
    // Calculate the number of remaining records in the array
    int remainingRecords = numRecords - i;
    // Determine the size of the current chunk, which is the minimum between 'chunkSize' and the remaining records
    int currentChunkSize = remainingRecords < chunkSize ? remainingRecords : chunkSize;

    // Copy the chunk of data to the chunkData array using 'memcpy' function
    // 'chunkData' is a temporary array used to store the current chunk of data for processing
    // 'vehicleData' is the original array containing all the vehicle position data
    // 'i' is the starting index of the current chunk in the original array
    // 'currentChunkSize' is the number of records to copy, multiplied by the size of each record (sizeof(struct VehiclePosition))
    memcpy(chunkData, &vehicleData[i], currentChunkSize * sizeof(struct VehiclePosition));

    // Find the closest position IDs for each given coordinate
    for (int k = 0; k < 10; k++) {
        // Get the latitude and longitude of the current given coordinate
        float givenLat = givenCoordinates[k].latitude;
        float givenLon = givenCoordinates[k].longitude;
        double minDistance = DBL_MAX;   // Variable to store the minimum distance found
        int closestPositionId = 0;     // Variable to store the position ID of the closest position

 // Process the data in parallel using OpenMP
        #pragma omp parallel for
        // Iterate over the chunkData array to find the closest position to the current given coordinate
        for (int j = 0; j < currentChunkSize; j++) {
            // Get the latitude and longitude of the current vehicle position in the chunk
            float vehicleLat = chunkData[j].latitude;
            float vehicleLon = chunkData[j].longitude;
            // Calculate the distance between the current vehicle position and the given coordinate
            double distance = haversineDistance(givenLat, givenLon, vehicleLat, vehicleLon);

            // Check if the distance is smaller than the current minimum distance
            // If so, update the minimum distance and store the position ID of the closest position
            if (distance < minDistance) {
                minDistance = distance;
                closestPositionId = chunkData[j].positionId;
            }
        }
        // At the end of this loop, 'closestPositionId' will contain the position ID of the closest position
        // to the current given coordinate 'k'.
// The loop continues with the next chunk until all records have been processed.
// Store the closest position ID in the main array
            closestPositions[k] = closestPositionId;
        }
    }

    // End timing
    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double executionTime = seconds + microseconds / 1e6;

    // Close the file and free allocated memory
    fclose(file);
    free(vehicleData);
    free(chunkData);

    // Print the closest position IDs for each given coordinate
    printf("Closest position IDs for each given coordinate:\n");
    for (int i = 0; i < 10; i++) {
        printf("Given Coordinate %d: %d\n", i + 1, closestPositions[i]);
        fflush(stdout); // Flush output buffer
    }

    // Print the execution time
    printf("Execution time: %.3f ms\n", executionTime * 1000.0);

    return 0;
}
