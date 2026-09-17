"""
tas_example_win.py
Windows-compatible version using Scapy 2.x

Requirements:
- Npcap installed (WinPcap compatible mode)
- Python venv with scapy installed
"""

import sys
import time
import ctypes

CYCLE_TIME_S = 0.025  # 25ms

# Windows' default system timer resolution is ~15.6ms, so time.sleep() with sub-15ms
# durations (used below for inter-packet spacing and cycle-boundary sync) can overshoot
# wildly and unpredictably. Raise it to 1ms for the life of this process.
winmm = ctypes.WinDLL('winmm')
winmm.timeBeginPeriod(1)

# ── Import Scapy safely ────────────────────────────────────────────────────────
try:
    from scapy.all import get_if_list, get_if_hwaddr, conf

    try:
        from scapy.arch.windows import get_windows_if_list
        HAS_WINDOWS_IF_LIST = True
    except ImportError:
        HAS_WINDOWS_IF_LIST = False
        
except ImportError as e:
    print("[ERROR] Failed to import Scapy")
    print("Details:", e)
    print("Make sure Scapy is installed in the current environment")
    sys.exit(1)


# ── Interface Selection ────────────────────────────────────────────────────────
def select_interface():
    """List only main Ethernet interfaces"""
    print("\n" + "=" * 55)
    print("Available Ethernet Interfaces")
    print("=" * 55)
    
    ethernet_ifaces = []
    
    if HAS_WINDOWS_IF_LIST:
        try:
            win_ifaces = get_windows_if_list()
            
            for iface in win_ifaces:
                name = iface.get('name', '')
                description = iface.get('description', '').lower()
                mac = iface.get('mac', '')
                
                # Skip filter layers and virtual adapters
                skip_keywords = [
                    'wfp', 'npcap', 'qos', 'filter', 'lightweight',
                    'loopback', 'virtual', 'vpn', 'bluetooth', 
                    'vmware', 'virtualbox', 'hyper-v', 'wsl',
                    'teredo', 'isatap', '6to4', 'kernel debug',
                    'wireless'
                ]
                
                if any(kw in description for kw in skip_keywords):
                    continue
                
                # Skip if MAC is empty or all zeros
                if not mac or mac == '00:00:00:00:00:00':
                    continue
                
                # Only show Ethernet interfaces
                ethernet_keywords = ['ethernet', 'realtek', 'intel', 'broadcom', 
                                    'gigabit', 'usb', 'asix', 'ax88']
                
                if any(kw in description for kw in ethernet_keywords):
                    ethernet_ifaces.append({
                        'name': name,
                        'description': iface.get('description', 'N/A'),
                        'mac': mac,
                        'guid': iface.get('guid', '')
                    })
            
            # Display filtered interfaces
            for i, iface in enumerate(ethernet_ifaces):
                print(f"\n  [{i}] {iface['name']}")
                print(f"      MAC: {iface['mac']}")
            
            if not ethernet_ifaces:
                print("\n  No Ethernet interfaces found!")
                return select_interface_fallback()
                
            print("\n" + "=" * 55)
            
            choice = input("\nSelect interface number: ").strip()
            idx = int(choice)
            selected = ethernet_ifaces[idx]
            iface_name = f"\\Device\\NPF_{selected['guid']}"
            
            return iface_name, selected['mac']
            
        except Exception as e:
            print(f"Error: {e}")
            return select_interface_fallback()
    else:
        return select_interface_fallback()


def select_interface_fallback():
    """Fallback to basic interface list"""
    ifaces = get_if_list()
    
    for i, iface in enumerate(ifaces):
        try:
            mac = get_if_hwaddr(iface)
        except:
            mac = "N/A"
        print(f"  [{i}] {iface} ({mac})")
    
    choice = input("\nSelect interface number: ").strip()
    idx = int(choice)
    selected = ifaces[idx]
    return selected, get_if_hwaddr(selected)


# ── Packet builder ────────────────────────────────────────────────────────────
def make_packet(mac_dst: str, mac_src: str, etype: str, payload_byte: str, repeat: int) -> bytes:
    dst     = bytes.fromhex(mac_dst)
    src     = bytes.fromhex(mac_src)
    et      = bytes.fromhex(etype)
    payload = bytes.fromhex(payload_byte) * repeat
    return dst + src + et + payload


# ── Main ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    print("=" * 55)
    print("TAS test script")
    print("=" * 55)
    
    iface, mac = select_interface()
    
    print(f"\nUsing: {iface}")
    print(f"MAC: {mac}")
    print("\nSending packets... (Ctrl+C to stop)\n")

    # Create packets
    pkt1 = make_packet("44444400000A", "70B3D5340000", "03E8", "11", 1500)
    pkt2 = make_packet("44444400000B", "70B3D5340000", "03E8", "11", 1500)
    pkt3 = make_packet("44444400000C", "70B3D5340000", "03E8", "11", 1500)
    pkt4 = make_packet("44444400000D", "70B3D5340000", "03E8", "11", 1500)
    time.sleep(0.025)

    # sendp() opens a fresh raw socket every call -- latency per call is several ms and
    # wildly inconsistent, swamping the intended 1ms inter-packet spacing. Open one socket
    # up front and reuse it for every send instead.
    tx_socket = conf.L2socket(iface=iface)

    count = 0
    next_time = time.perf_counter()

    try:
        while True:
            # Send exactly 1 frame per IPV per cycle
            # Small spacing between packets to avoid bursting them, which skews gate timing
            tx_socket.send(pkt1)
            time.sleep(0.001)
            tx_socket.send(pkt2)
            time.sleep(0.001)
            tx_socket.send(pkt3)
            time.sleep(0.001)
            tx_socket.send(pkt4)

            count += 4
            if count % 100 == 0:
                print(f"Sent {count} packets")

            # Sync with the TAS cycle -- sleep until the next cycle boundary
            next_time += CYCLE_TIME_S
            sleep_time = next_time - time.perf_counter()
            if sleep_time > 0:
                time.sleep(sleep_time)
            else:
                # Fell behind (OS/driver stutter) -- resync to now instead of bursting
                # through the backlog of missed cycles, which scrambles gate order.
                next_time = time.perf_counter()

    except KeyboardInterrupt:
        print(f"\nFinished. Total sent: {count} packets")
    finally:
        tx_socket.close()
        winmm.timeEndPeriod(1)