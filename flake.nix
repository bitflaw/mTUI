{
  description = "minimal TUI";
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-26.05";
  };
  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          cmake
          clang-tools
        ];
      };
    };
}
