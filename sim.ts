namespace FastTFT128 {
    export namespace api {
        // let simCanvas: HTMLCanvasElement = null
        // let ctx: CanvasRenderingContext2D = null
        let fb: number[] = []

        function ensure() {
            // if (!simCanvas) {
            //     simCanvas = document.createElement("canvas")
            //     simCanvas.width = 128
            //     simCanvas.height = 128
            //     simCanvas.style.width = "256px"
            //     simCanvas.style.height = "256px"
            //     simCanvas.style.imageRendering = "pixelated"
            //     document.body.appendChild(simCanvas)
            //     ctx = simCanvas.getContext("2d")
                 fb = new Array(128 * 128)
                clearFramebuffer(0)
            // }
        }

        export function init() { ensure() }
        export function setSPISpeed(hz: number) { ensure() }
        export function rgb(red: number, green: number, blue: number): number {
            return ((red & 0xf8) << 8) | ((green & 0xfc) << 3) | (blue >> 3)
        }
        function css(c: number): string {
            let r = ((c >> 11) & 31) * 255 / 31
            let g = ((c >> 5) & 63) * 255 / 63
            let b = (c & 31) * 255 / 31
            return `rgb(${r},${g},${b})`
        }
        export function clear(color: number) { 
            // ensure(); 
            // ctx.fillStyle=css(color); 
            // ctx.fillRect(0,0,128,128) 
        }
        export function fillRect(x:number,y:number,w:number,h:number,color:number) {
            //  ensure(); 
            //  ctx.fillStyle=css(color); 
            //  ctx.fillRect(x,y,w,h) 
            }
        export function pixel(x:number,y:number,color:number) { fillRect(x,y,1,1,color) }
        export function hLine(x:number,y:number,l:number,c:number) { fillRect(x,y,l,1,c) }
        export function vLine(x:number,y:number,l:number,c:number) { fillRect(x,y,1,l,c) }
        export function createFramebuffer() { ensure(); if (!fb.length) fb=new Array(16384) }
        export function clearFramebuffer(c:number) { createFramebuffer(); for(let i=0;i<fb.length;i++) fb[i]=c }
        export function setPixel(x:number,y:number,c:number) { createFramebuffer(); if(x>=0&&x<128&&y>=0&&y<128) fb[y*128+x]=c }
        export function fillFramebufferRect(x:number,y:number,w:number,h:number,c:number) { createFramebuffer(); for(let yy=Math.max(0,y);yy<Math.min(128,y+h);yy++) for(let xx=Math.max(0,x);xx<Math.min(128,x+w);xx++) fb[yy*128+xx]=c }
        export function show() { 
            createFramebuffer(); 
            // for(let y=0;y<128;y++) {
            //     for(let x=0;x<128;x++) { 
            //         ctx.fillStyle=css(fb[y*128+x]||0); 
            //         ctx.fillRect(x,y,1,1)
            //     } 
            // }   
        }
    }
}
