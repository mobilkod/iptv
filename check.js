const fs = require("fs");

const inputFile = "index.m3u8";
const outputFile = "valid.m3u8";
const concurrency = 20;

async function checkUrl(url) {
    try {
        const response = await fetch(url, {
            method: "HEAD",
            redirect: "follow",
            signal: AbortSignal.timeout(10000)
        });

        return response.ok;
    } catch {
        return false;
    }
}

async function main() {
    const content = fs.readFileSync(inputFile, "utf8");
    const lines = content.split(/\r?\n/);

    const results = [];
    let currentInfo = null;

    // M3U formatındaki #EXTINF satırlarını linkleriyle birlikte koru
    for (const line of lines) {
        const trimmed = line.trim();

        if (trimmed.startsWith("#EXTINF")) {
            currentInfo = line;
            continue;
        }

        if (/^https?:\/\//i.test(trimmed)) {
            results.push({
                info: currentInfo,
                url: trimmed
            });

            currentInfo = null;
        }
    }

    console.log(`Toplam link: ${results.length}`);

    const working = [];

    // Aynı anda sınırlı sayıda link kontrol et
    for (let i = 0; i < results.length; i += concurrency) {
        const batch = results.slice(i, i + concurrency);

        const checked = await Promise.all(
            batch.map(async item => ({
                ...item,
                working: await checkUrl(item.url)
            }))
        );

        for (const item of checked) {
            if (item.working) {
                console.log(`✅ ${item.url}`);
                working.push(item);
            } else {
                console.log(`❌ ${item.url}`);
            }
        }
    }

    // Yeni M3U8 oluştur
    let output = "#EXTM3U\n";

    for (const item of working) {
        if (item.info) {
            output += item.info + "\n";
        }

        output += item.url + "\n";
    }

    fs.writeFileSync(outputFile, output, "utf8");

    console.log("\n--------------------------------");
    console.log(`Çalışan: ${working.length}`);
    console.log(`Çalışmayan: ${results.length - working.length}`);
    console.log(`Yeni dosya: ${outputFile}`);
}

main().catch(console.error);
