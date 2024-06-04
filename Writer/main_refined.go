package main

import (
	"bufio"
	"encoding/hex"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)


//Written by : @Rhythm113 (赛高)



//錯誤處理 
func checkError(err error, message string) {
	if err != nil {
		fmt.Printf("%s: %v\n", message, err)
		os.Exit(1)
	}
}

//基底地址 
func getModuleBase(moduleName string, targetPID int) (uintptr, error) {
	var addr uintptr
	filename := fmt.Sprintf("/proc/%d/maps", targetPID)
	file, err := os.Open(filename)
	checkError(err, "Error opening maps file")
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.Contains(line, moduleName) {
			_, err := fmt.Sscanf(strings.Split(line, "-")[0], "%x", &addr)
			checkError(err, "Error parsing module base address")
			return addr, nil
		}
	}

	return 0, fmt.Errorf("Module not found in maps")
}

//結束添加
func getModuleEnd(moduleName string, targetPID int) (uintptr, error) {
	var addr uintptr
	filename := fmt.Sprintf("/proc/%d/maps", targetPID)
	file, err := os.Open(filename)
	checkError(err, "Error opening maps file")
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.Contains(line, moduleName) {
			_, err := fmt.Sscanf(strings.Split(line, "-")[1], "%x", &addr)
			checkError(err, "Error parsing module end address")
			return addr, nil
		}
	}

	return 0, fmt.Errorf("Module not found in maps")
}


//使用系統寫入寫入數據
func writeToProcessMemory(pid int, base uintptr, offset uint64, data []byte) {
	memPath := fmt.Sprintf("/proc/%d/mem", pid)
	memFile, err := os.OpenFile(memPath, os.O_WRONLY, 0)
	checkError(err, "Error opening mem file")
	defer memFile.Close()

	startTime := time.Now()

	_, err = memFile.WriteAt(data, int64(base+uintptr(offset))) //vm_writev 替代實施 
	checkError(err, "Error writing to process memory")

	elapsed := time.Since(startTime).Milliseconds()
	fmt.Printf("Write successful to process %d. Written buffer: %s\nTime taken: %d ms\n", pid, hex.EncodeToString(data), elapsed)
}

//取得PID 
func getPID(packageName string) (int, error) {
	dir, err := ioutil.ReadDir("/proc")
	checkError(err, "Error reading /proc")

	for _, entry := range dir {
		if !entry.IsDir() {
			continue
		}

		cmdlinePath := filepath.Join("/proc", entry.Name(), "cmdline")
		filetext, err := ioutil.ReadFile(cmdlinePath)
		if err != nil {
			continue
		}

		processName := strings.TrimRight(string(filetext), "\x00")
		if processName == packageName {
			pid, err := strconv.Atoi(entry.Name())
			checkError(err, "Error converting PID")
			if pid < 10 {
				return 0, fmt.Errorf("PID is less than 10")
			}
			return pid, nil
		}
	}

	return 0, fmt.Errorf("Process not found with the given package name")
}

//主要功能
func main() {
	if len(os.Args) < 5 {
		fmt.Println("Kernel Based Writing (v1.0-stable)\nBy:@Rhythm113\nUsage: su -c write <lib_name> <package_name> <offset> <hex_without_spaces>")
		os.Exit(1)
	}

	moduleName := os.Args[1]
	packageName := os.Args[2]
	targetPID, err := getPID(packageName)
	checkError(err, "Error getting target PID")
	offset, err := strconv.ParseUint(os.Args[3], 0, 64)
	checkError(err, "Error parsing offset")
	dataHex := os.Args[4]
	data, err := hex.DecodeString(dataHex)
	checkError(err, "Error decoding hex string")

	base, err := getModuleBase(moduleName, targetPID)
	checkError(err, "Error getting module base address")


	writeToProcessMemory(targetPID, base, offset, data)
}
