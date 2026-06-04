import socket
import threading
import time
import argparse
 
# ─── Configuration ────────────────────────────────────────────────────────────
 
RECONNECT_DELAY  = 1.0
SEND_INTERVAL    = 1.0
RECV_BUFFER_SIZE = 256
 
# ─── Worker ───────────────────────────────────────────────────────────────────
 
def tcp_client_worker(server_ip: str, server_port: int, name: str) -> None:
    msg_id = 0
 
    while True:
        print(f"[{name}] Connecting to {server_ip}:{server_port} ...")
 
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.settimeout(5.0)
                sock.connect((server_ip, server_port))
                print(f"[{name}] Connected.")
 
                while True:
                    msg = f"Hello from PC | ID={msg_id}"
                    sock.sendall(msg.encode("utf-8"))
                    print(f"[{name}] Sent     : {msg}")
 
                    try:
                        data = sock.recv(RECV_BUFFER_SIZE)
                        if not data:
                            print(f"[{name}] Server closed connection.")
                            break
                        print(f"[{name}] Echo     : {data.decode('utf-8', errors='replace')}")
                    except socket.timeout:
                        print(f"[{name}] Recv timeout (no echo).")
                        break
 
                    msg_id += 1
                    time.sleep(SEND_INTERVAL)
 
        except (ConnectionRefusedError, TimeoutError, OSError) as e:
            print(f"Connection is down. Please wait for reconnection or reset the RA board. Retrying in {RECONNECT_DELAY}s...")
 
        time.sleep(RECONNECT_DELAY)
 
 
# ─── Helpers ──────────────────────────────────────────────────────────────────
 
def parse_address(addr: str) -> tuple[str, int]:
    try:
        ip, port_str = addr.rsplit(":", 1)
        port = int(port_str)
        if not (1 <= port <= 65535):
            raise ValueError
        return (ip, port)
    except ValueError:
        raise argparse.ArgumentTypeError(
            f"Invalid address '{addr}'. Expected format IP:PORT (e.g. 192.168.0.101:5008)"
        )
 
 
# ─── Entry point ──────────────────────────────────────────────────────────────
 
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="TCP Client — 1 or 2 servers, persistent connection",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog=(
            "Examples:\n"
            "  python tcp_client.py 192.168.0.101:5008\n"
            "  python tcp_client.py 192.168.0.101:5008 192.168.0.102:5008\n"
            "  python tcp_client.py 192.168.0.101:5008 --interval 0.5\n"
        )
    )
 
    parser.add_argument("servers",
                        metavar="IP:PORT",
                        type=parse_address,
                        nargs="+",
                        help="Server address(es), e.g. 192.168.0.101:5008")
    parser.add_argument("--interval",  type=float, default=SEND_INTERVAL,   metavar="SEC", help=f"Send interval in seconds (default: {SEND_INTERVAL})")
    parser.add_argument("--reconnect", type=float, default=RECONNECT_DELAY, metavar="SEC", help=f"Reconnect delay in seconds (default: {RECONNECT_DELAY})")
 
    args = parser.parse_args()
 
    if len(args.servers) > 2:
        parser.error("Too many arguments. Maximum 2 servers allowed.")
 
    SEND_INTERVAL   = args.interval
    RECONNECT_DELAY = args.reconnect
 
    print("=" * 50)
    print("         TCP Client — Server Config")
    print("  Mode : persistent connection")
    print("=" * 50)
    for i, (ip, port) in enumerate(args.servers, start=1):
        print(f"  Server {i} : {ip}:{port}")
    print(f"  Interval : {SEND_INTERVAL}s")
    print("=" * 50 + "\n")
 
    threads = []
    for i, (ip, port) in enumerate(args.servers, start=1):
        name = f"TCP Client"
        t = threading.Thread(
            target=tcp_client_worker,
            args=(ip, port, name),
            name=name,
            daemon=True
        )
        t.start()
        threads.append(t)
        print(f"[Main] Started thread for {name} ({ip}:{port})")
 
    print("[Main] Press Ctrl+C to stop.\n")
 
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[Main] Shutting down.")