package fasthtml.benchmark;

import fasthtml.FastHTML;
import org.openjdk.jmh.annotations.*;

import java.nio.charset.StandardCharsets;
import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.Throughput)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Benchmark)
@Warmup(iterations = 2, time = 1, timeUnit = TimeUnit.SECONDS)
@Measurement(iterations = 3, time = 1, timeUnit = TimeUnit.SECONDS)
@Fork(1)
public class Benchmark {

    private FastHTML engine;
    private byte[] sampleHtml;

    @Setup
    public void setup() {
        engine = FastHTML.open();
        String html = "<!DOCTYPE html><html><head><title>Test</title><script>alert('xss');</script></head>" +
                "<body><h1>Hello World</h1><p>This is a <a href='https://example.com' onclick='steal()'>link</a>.</p>" +
                "<style>body { color: red; }</style><div class='content'>Safe content here.</div></body></html>";
        sampleHtml = html.getBytes(StandardCharsets.UTF_8);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public String benchmarkSanitizeStrict() {
        return engine.sanitize(sampleHtml, FastHTML.SafetyProfile.STRICT_WHITELIST);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public String benchmarkSanitizeLenient() {
        return engine.sanitize(sampleHtml, FastHTML.SafetyProfile.LENIENT_BLACKLIST);
    }

    @org.openjdk.jmh.annotations.Benchmark
    public String benchmarkSanitizeTextOnly() {
        return engine.sanitize(sampleHtml, FastHTML.SafetyProfile.TEXT_ONLY);
    }
}
