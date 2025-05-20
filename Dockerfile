# Use an official Ubuntu image as a parent image
# ubuntu:22.04 (Jammy Jellyfish) is a good recent LTS choice
FROM ubuntu:22.04

# Set DEBIAN_FRONTEND to noninteractive to avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update package lists and install core development tools and MiniLibX dependencies
# build-essential: includes gcc, g++, make, etc.
# git: for version control
# vim: a common text editor
# libx11-dev, libxext-dev: Core X11 libraries
# libbsd-dev: for functions like strlcpy/strlcat if not in local libft or used by mlx
# xorg-dev: broader Xorg development package
# libxft-dev: X FreeType library, sometimes needed for font rendering with MiniLibX
# libxpm-dev: X PixMap library, often used for .xpm texture files with MiniLibX
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    git \
    vim \
    libx11-dev \
    libxext-dev \
    libbsd-dev \
    xorg-dev \
    libxft-dev \
    libxpm-dev \
    # Clean up apt cache to reduce image size
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user for better security and practice
# -m: create home directory
# -s /bin/bash: set default shell
RUN useradd -ms /bin/bash cub3d_user

# Set the working directory in the container
WORKDIR /app

# Switch to the non-root user
USER cub3d_user

# Default command to run when a container starts from this image
# This provides an interactive bash shell by default.
CMD ["bash"]
