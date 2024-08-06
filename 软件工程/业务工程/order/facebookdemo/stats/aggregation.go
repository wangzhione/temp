package stats

import "sort"

// Average returns the average value
func Average(values []float64) float64 {
	if len(values) == 0 {
		return 0
	}
	return Sum(values) / float64(len(values))
}

// Sum returns the sum of all the given values
func Sum(values []float64) float64 {
	var sum float64
	for _, value := range values {
		sum += value
	}
	return sum
}

// Percentiles returns a map containing the asked for percentiles
func Percentiles(values []float64, percentiles map[string]float64) map[string]float64 {
	sort.Float64s(values)
	result := map[string]float64{}
	for label, percentile := range percentiles {
		result[label] = values[int(float64(len(values))*percentile)]
	}
	return result
}
