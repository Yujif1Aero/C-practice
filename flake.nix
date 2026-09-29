{
  description = "OpenLB GCC/MPI/OpenMP and CUDA development shells";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
      };

      gccMpiOmp = pkgs.mkShell {
        packages = with pkgs; [ gnumake openmpi ];
      };

      cuda = (pkgs.mkShell.override { stdenv = pkgs.stdenvNoCC; }) {
        packages = with pkgs; [
          gnumake
          cudaPackages.backendStdenv.cc
          cudaPackages.cuda_nvcc
          cudaPackages.cuda_cudart
          cudaPackages.cuda_cccl
        ];
      };
    in {
      devShells.${system} = {
        default = gccMpiOmp;
        gcc-mpi-omp = gccMpiOmp;
        inherit cuda;
      };
    };
}
