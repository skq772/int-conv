{ pkgs ? import <nixpkgs> { } }:

with pkgs; mkShell {
  nativeBuildInputs = [
    cmake pkg-config
  ];
  buildInputs = with xorg; [
    libGL libXrandr libXinerama libXcursor libX11 libXi libXext
  ];
}
