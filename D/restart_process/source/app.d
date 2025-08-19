import std.process;
import core.thread;
import std.stdio;
import std.file;
import std.path;

void main() {
	string current_path = thisExePath().dirName();
    string processName = current_path ~ "\\learn-d.exe"; 

	writeln("Monitoring Process: ", processName);
	
	while (true) {
	    auto pid = spawnShell(processName); 
	    writeln("Process Launched: ", pid);
	
	    int status = wait(pid); 
	    writeln("Process Terminated: ", pid, " Status: ", status);
	
	    writeln("Restarting Process...");
	    Thread.sleep(1.seconds); 
	}
}