with import <nixpkgs> {};

lib.fix (self: {
  z80e = stdenv.mkDerivation {
     name = "z80e";
     src = fetchurl {
       url = "https://github.com/KnightOS/z80e/archive/0.4.0.tar.gz";
       sha256 = "b8940393bfff843ef26b2f43770a9d7e8cb0601f67804571b238bbca52919e7d";
     };
     buildInputs = [ self.scas cmake readline SDL ];
     cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release -Denable-sdl=YES" ];
  };
       
  scas = stdenv.mkDerivation {
     name = "scas";
      src = fetchurl {
        url = "https://github.com/KnightOS/scas/archive/0.3.3.tar.gz";
        sha256 = "11072245e09129d283d0f749a511c04930536e96ce05065da03fd70047fb0c10";
      };
     hardeningDisable = [ "format" ];
     nativeBuildInputs = [ cmake ];
     patchPhase = "patchShebangs generate-tables.sh";
     cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];
  };
  }
)
