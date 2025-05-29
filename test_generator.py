import random
import time

class ProcessData:
    def __init__(self):
        self.arrivaltime = 1
        self.priority = 0
        self.runningtime = 0
        self.id = 0
        self.memsize = 1

def main():
    # Open file for writing
    with open("processes.txt", "w") as pFile:
        # Get number of processes from user
        no = int(input("Please enter the number of processes you want to generate: "))
        
        # Seed random generator
        random.seed(time.time())
        
        # Write header to file
        pFile.write("#id arrival runtime priority memsize\n")
        
        # Initialize process data
        pData = ProcessData()
        
        # Generate and write data for each process
        for i in range(1, no + 1):
            # Generate data randomly
            pData.id = i
            pData.arrivaltime += random.randint(0, 10)  # processes arrive in order
            pData.runningtime = random.randint(0, 29)   # equivalent to rand() % 30
            pData.priority = random.randint(0, 10)      # equivalent to rand() % 11
            pData.memsize = random.randint(0, 255)      # equivalent to rand() % 256
            
            # Write data to file
            pFile.write(f"{pData.id}\t{pData.arrivaltime}\t{pData.runningtime}\t{pData.priority}\t{pData.memsize}\n")

if __name__ == "__main__":
    main()