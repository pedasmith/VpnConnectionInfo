// VpnConnectionInfo.cpp : tiny utility to pull live VPN connection statistics
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ras.h>
#include <RasError.h>
#include <stdio.h>
#include <malloc.h>

#pragma comment(lib, "rasapi32.lib")

/// <summary>
/// Displays data about the VPN connections
/// </summary>
/// <returns></returns>
int wmain(int argc, wchar_t**argv)
{
	int retval = 0;
	const int NCONN = 1000; // May as well be much larger than will ever be needed.
	const int CONN_NBYTES = NCONN * sizeof(RASCONNW);

	wchar_t* requiredMatch = 0;
	for (int i = 1; i < argc; i++)
	{
		auto arg = argv[i];

		if (wcscmp(arg, L"/name") == 0)
		{
			requiredMatch = argv[i + 1];
			i++;
		}
		else if (wcscmp(arg, L"/?") == 0)
		{
			printf("VpnConnectionInfo: display information about connected VPNs\n");
			printf("    /name <name> : only print information for VPNs whose phonename name matches the given name. Names must match exactly\n");
			printf("Return value: 0=success 1=no matching name found 10=unable to allocation memory 11=bad command syntax");

			return 0;
		}
		else
		{
			wprintf(L"VpnConnectionInfo: Error: bad syntax for arg <<%s>> %d\n", arg, i);
			printf("    /name <name> : only print information for VPNs whose phonename name matches the given name. Names must match exactly\n");
			printf("Return value: 0=success 1=no matching name found 10=unable to allocation memory 11=bad command syntax");
			return 11;
		}
	}

	RASCONNW* rasConns = (RASCONNW*)malloc(sizeof(RASCONNW) * NCONN); // Better code would make this more dynamic;
	if (rasConns == 0)
	{
		// Seriously this can't ever happen
		fprintf(stderr, "ERROR: VonConnectionInfo: unable to allocate connection array\n");
		return 10; 
	}
	rasConns[0].dwSize = sizeof(RASCONNW);
	DWORD rasConnsSize = CONN_NBYTES;
	DWORD nEntry = 0;
	RasEnumConnectionsW(rasConns, &rasConnsSize, &nEntry);
	int nprint = 0;

	printf("Index\tEntryName\tDeviceName\tRecv\tSent\n");
	for (DWORD i = 0; i < nEntry; i++)
	{
		auto entry = rasConns[i];
		auto h = entry.hrasconn;
		RAS_STATS connStats;
		connStats.dwSize = sizeof(connStats);
		RasGetConnectionStatistics(h, &connStats);
		auto canPrint = requiredMatch == 0 || wcscmp(requiredMatch, entry.szEntryName) == 0;
		if (canPrint)
		{
			printf("%d\t%S\t%S\t%d\t%d\n", i, entry.szEntryName, entry.szDeviceName, connStats.dwBytesRcved, connStats.dwBytesXmited);
			nprint++;
		}
	}

	if (requiredMatch != 0 && nprint == 0)
	{
		fprintf(stderr, "Error: no matches for %S\n", requiredMatch);
		retval = 1;
	}

	return retval;
}

