package stats

import (
	"encoding/json"
	"testing"
)

func TestSimpleCounterAggregation(t *testing.T) {
	t.Parallel()

	a := Aggregates{}
	a.Add(&SimpleCounter{
		Key:    "foo.avg",
		Values: []float64{1},
		Type:   AggregateAvg,
	})
	a.Add(&SimpleCounter{
		Key:    "foo.sum",
		Values: []float64{1},
		Type:   AggregateSum,
	})
	a.Add(&SimpleCounter{
		Key:    "foo.time",
		Values: []float64{0, 1, 2, 3, 4},
		Type:   AggregateHistogram,
	})
	a.Add(&SimpleCounter{
		Key:    "foo.sum",
		Values: []float64{2},
		Type:   AggregateSum,
	})
	a.Add(&SimpleCounter{
		Key:    "foo.time",
		Values: []float64{5, 6, 7, 8, 9, 10},
		Type:   AggregateHistogram,
	})
	a.Add(&SimpleCounter{
		Key:    "foo.avg",
		Values: []float64{2},
		Type:   AggregateAvg,
	})

	all := map[string]float64{}
	for _, counter := range a {
		for key, value := range counter.(*SimpleCounter).Aggregate() {
			all[key] = value
		}
	}

	data, err := json.MarshalIndent(all, "", "")
	if err != nil {
		t.Fatalf("json.MarshalIndent error = %+v", err)
	}
	t.Logf("all = \n%s\n", data)

	expected := map[string]float64{
		"foo.avg":      1.5,
		"foo.sum":      3.0,
		"foo.time":     5.0,
		"foo.time.p50": 5.0,
		"foo.time.p95": 10.0,
		"foo.time.p99": 10.0,
	}

	DeepEqual(t, len(all), len(expected))

	for key, value := range all {
		DeepEqual(t, value, expected[key])
	}
}
