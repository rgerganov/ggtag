from distutils.core import setup, Extension

def main():
    setup(name="ggtag",
          version="0.0.5",
          description="Python bindings for ggtag",
          long_description="Python bindings for ggtag",
          author="Radoslav Gerganov",
          author_email="rgerganov@gmail.com",
          include_dirs = ['host/include', 'shared/include'],
          py_modules = ['ggtag'],
          url="https://ggtag.io",
          ext_modules=[Extension("cggtag", [
                "cggtag.c",
                "host/src/ggtag.cpp",
                "host/src/utils.cpp",
                "host/src/rfid.cpp",
                "shared/src/protocol.cpp",
                "shared/src/GUI_Paint.c",
                "shared/src/fa.c",
                "shared/src/font8.c",
                "shared/src/font12.c",
                "shared/src/font16.c",
                "shared/src/font20.c",
                "shared/src/font24.c",
                "shared/src/qrcodegen.c",
                ])])

if __name__ == "__main__":
    main()
