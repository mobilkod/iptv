const puppeteer = require("puppeteer");

const pageUrl = "https://www.todtv.com.tr/canli-tv/national_geo_wild?c=belgesel";

(async () => {
    const browser = await puppeteer.launch({
        headless: false
    });

    const page = await browser.newPage();

    page.on("request", req => {
        const url = req.url();

        if (
            url.includes(".mpd") ||
            url.includes(".m3u8")
        ) {
            console.log("DASH/HLS REQUEST:");
            console.log(url);
        }
    });

    page.on("response", async res => {
        const url = res.url();

        if (url.includes(".mpd")) {
            console.log("MPD:", res.status(), url);
        }
    });

    await page.goto(pageUrl, {
        waitUntil: "domcontentloaded",
        timeout: 60000
    });

    // Player'ın başlaması için bekle
    await new Promise(r => setTimeout(r, 15000));

    console.log("Tarayıcı açık...");
})();
