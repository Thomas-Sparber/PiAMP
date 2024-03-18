const fs = require('fs');
const bmp = require('bmp-js');

class ImageReader {

    imageFolder;
    lastName;
    bmpData;

    constructor(imageFolder) {
        this.imageFolder = imageFolder;
    }

    getImageData(name, x, y, w, h) {
        if(name != this.lastName) {
            var filename = this.imageFolder + "/" + name + ".bmp";
            if(!fs.existsSync(filename)) {
                console.log("No image found for " + name);
                filename = this.imageFolder + "/noimage.bmp";
            }

            var bmpBuffer = fs.readFileSync(filename);
            this.bmpData = bmp.decode(bmpBuffer);
            this.lastName = name;
        }

        var buffer = Buffer.alloc(w * h * 3);
        var counter = 0;

        for(var y_counter = y; y_counter < y+h; y_counter++) {
            for(var x_counter = x; x_counter < x+w; x_counter++) {
                var index = (this.bmpData.width * y_counter + x_counter) * 4;
                var r = this.bmpData.data[index + 3];
                var g = this.bmpData.data[index + 2];
                var b = this.bmpData.data[index + 1];
                buffer[counter++] = r;
                buffer[counter++] = g
                buffer[counter++] = b

                //console.log("Image data: ("+x_counter+","+y_counter+"): ("+r+","+g+","+b+")");
            }
        }

        return buffer;
    }

}

module.exports = ImageReader;
