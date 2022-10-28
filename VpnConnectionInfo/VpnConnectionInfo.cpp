// VpnConnectionInfo.cpp : tiny utility to pull live VPN connection statistics
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ras.h>
#include <RasError.h>
#include <stdio.h>

#pragma comment(lib, "rasapi32.lib")

int main()
{
	int retval = 0;

	RASCONNW rasConns[50]; // Better code would make this more dynamic;
	rasConns[0].dwSize = sizeof(RASCONNW);
	DWORD rasConnsSize = sizeof(rasConns);
	DWORD nEntry = 0;
	RasEnumConnectionsW(rasConns, &rasConnsSize, &nEntry);

	printf("VpnConnectionInfo: #connections:%d\n", nEntry);
	for (DWORD i = 0; i < nEntry; i++)
	{
		auto entry = rasConns[i];
		printf("    %d: connection entry name=%S device name=%S\n", i, entry.szEntryName, entry.szDeviceName);
		auto h = entry.hrasconn;
		RAS_STATS connStats;
		connStats.dwSize = sizeof(connStats);
		RasGetConnectionStatistics(h, &connStats);
		printf("    %d: stats recv=%d sent=%d\n", i, connStats.dwBytesRcved, connStats.dwBytesXmited);
		//RasGetLinkStatistics(h, )
	}

	printf("Done\n");
	return retval;
}

