import importlib.metadata
import re
import sys
import subprocess
import requests
import logging
import argparse

logging.basicConfig(
    level=logging.DEBUG,
    format="%(asctime)s [%(levelname)s] (%(filename)s #%(lineno)d, %(funcName)s): %(message)s",
    handlers=[logging.StreamHandler()]
)
logger = logging.getLogger(__name__)


def install_package(pkg_spec: str):
    """Install or upgrade a package using pip"""
    logger.info(f"➡️ Installing: {pkg_spec}")
    result = subprocess.run([sys.executable, "-m", "pip", "install", pkg_spec])
    if result.returncode == 0:
        logger.info(f"✅ Successfully installed {pkg_spec}")
    else:
        logger.error(f"❌ Failed to install {pkg_spec}")


def load_requirements(url: str):
    logger.info(f"📥 Downloading requirements from {url}")
    try:
        r = requests.get(url, timeout=10)
        r.raise_for_status()
    except requests.exceptions.RequestException as e:
        logger.error(f"❌ Failed to download requirements: {e}")
        logger.info("Please check the internet connection. If that does not help, please contact Renesas technical support.")
        return []
    return r.text.splitlines()


def check_requirements(url: str, do_upgrade: bool = False):
    lines = load_requirements(url)

    for line in lines:
        line = line.strip()
        if not line or line.startswith("#"):
            continue

        # If the line is a .whl link
        if line.startswith("http"):
            filename = line.split("/")[-1]
            parts = filename.split("-")
            pkg_name = parts[0]
            pkg_version = parts[1]
            try:
                version = importlib.metadata.version(pkg_name)
                if version == pkg_version:
                    logger.info(f"{pkg_name}: INSTALLED ({version})")
                else:
                    logger.info(f"{pkg_name}: VERSION MISMATCH "
                                f"(installed {version}, required {pkg_version})")
                    if do_upgrade:
                        install_package(line)  # install with version pin
            except importlib.metadata.PackageNotFoundError:
                logger.info(f"{pkg_name}: NOT INSTALLED")
                if do_upgrade:
                    install_package(line)  # install from URL
            continue

        # If the line is in format package==version
        match = re.match(r"([a-zA-Z0-9_\-]+)==([0-9\.]+)", line)
        if match:
            pkg, required_version = match.groups()
            try:
                installed_version = importlib.metadata.version(pkg)
                if installed_version == required_version:
                    logger.info(f"{pkg}=={required_version}: OK")
                else:
                    logger.info(f"{pkg}: VERSION MISMATCH "
                                f"(installed {installed_version}, required {required_version})")
                    if do_upgrade:
                        install_package(line)  # install with version pin
            except importlib.metadata.PackageNotFoundError:
                logger.info(f"{pkg}: NOT INSTALLED")
                if do_upgrade:
                    install_package(line)
        else:
            logger.error(f"⚠️ Could not parse line: {line}")


def parse_args():
    parser = argparse.ArgumentParser(description="Script that accepts arguments with default values")
    parser.add_argument("--RequireLocation", type=str, default="", help="Requirement file location")
    parser.add_argument("--Update", type=str, default="OFF", help="Request update package")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    req_url = args.RequireLocation
    if not (req_url.startswith("http://") or req_url.startswith("https://")):
        logger.error("Error: requirements.txt must be provided as a URL (http/https)")
        sys.exit(1)

    do_upgrade = "ON" in args.Update
    check_requirements(req_url, do_upgrade)
