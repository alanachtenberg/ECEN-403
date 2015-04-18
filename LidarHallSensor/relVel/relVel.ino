 float time_buff[5];
 unsigned int lidar_distance[5];
 unsigned int relVel;
 
void setup() {
  // put your setup code here, to run once:

}

void loop() {

 
 int index=0;
 relVel = 0;
 time_buff[index] = 1000*millis();
 
 lidar_distance[index] = reading;
 
 
 if (index = 5)
 {
   relVel = 1/5*((lidar_distance[0]/time_buff[0])+(lidar_distance[1]/time_buff[1])+(lidar_distance[2]/time_buff[2])+lidar_distance[3]/time_buff[3])+lidar_distance[4]/time_buff[4]))
   index=0;
 }
 index++;

}
